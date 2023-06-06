/******************************************************************************
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2023 Valve Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/

#include "steamos.h"
#include "core/core.h"
#include "core/remote_server.h"
#include "replay/replay_driver.h"
#include <cpp-httplib/httplib.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <map>

struct SteamOSRemoteServer : public RemoteServer
{
  SteamOSRemoteServer(Network::Socket *sock, const rdcstr &deviceID, uint16_t portbase)
      : RemoteServer(sock, deviceID), m_portbase(portbase)
  {
  }

  virtual ~SteamOSRemoteServer() override {}
  virtual rdcstr GetHomeFolder() override { return ""; }
  virtual rdcarray<PathEntry> ListFolder(const rdcstr &path) override
  {
    // on the root path, fetch list of games
    if(path.empty() || (path[0] == '/' && path.size() == 1))
    {
      PathEntry g;
      g.size = 0;
      g.lastmod = 0;    // could fill these out but don't have to
      g.flags = PathProperty::Executable;

      rdcarray<PathEntry> games;
      for(const rdcstr &gameName : {"portal2", "myGame"})
      {
        g.filename = gameName;
        games.push_back(g);
      }

      // could also create fake directories here (android does that for two-tier, packages and
      // activities). Or hidden directories for programs you want to hide from the user by default
      // we could store appids here and look them up later by path in this virtual filesystem

      return games;
    }

    return {};
  }

  virtual ExecuteResult ExecuteAndInject(const rdcstr &steamtitle, const rdcstr &workingDir,
                                         const rdcstr &args,
                                         const rdcarray<EnvironmentModification> &env,
                                         const CaptureOptions &opts) override
  {
    ExecuteResult ret = {};

    // TODO: future? ask devkit somehow to launch the given via special magic means, passing along
    // the arguments
    // steamtitle will be the path in our fake filesystem, so /portal2
    // we can block here if we want.

    ret.result = ResultCode::Succeeded;
    ret.ident =
        RenderDoc_FirstTargetControlPort;    // should be the target control connection opened

    return ret;
  }

private:
  uint16_t m_portbase = 0;
};

struct SteamOSController : public IDeviceProtocolHandler
{
  // key: immutable (at least within a session/reboot) id
  // value: friendly name and base port assuming localhost port forward
  std::map<rdcstr, rdcpair<rdcstr, uint16_t>> devices;

  void UpdateDB()
  {
    devices.clear();
    httplib::Client cl("http://127.0.0.1:32010");
    httplib::Result res = cl.Get("/selected_devkit");
    if(!res)
    {
      RDCLOG("Query SteamOS devkit tool failed: %s", to_string(res.error()).c_str());
      return;
    }
    RDCLOG("DBG DBG %s", res->body.c_str());
    json selected_kit = json::parse(res->body);
    RDCLOG("DBG DBG %s", selected_kit["name"].dump().c_str());
/*
    // load from JSON
    devices = {
        {"dev12345", {"RetailKit", 25000}}, {"dev8675309", {"GitMesaKit", 25100}},
    };
    */
  }

  rdcstr GetProtocolName() override { return "steamos"; }
  rdcarray<rdcstr> GetDevices() override
  {
    UpdateDB();
    rdcarray<rdcstr> ret;
    for(auto it : devices)
      ret.push_back(it.first);
    return ret;
  }
  rdcstr GetFriendlyName(const rdcstr &URL) override
  {
    // URL will be steamos://dev12345 for example so look up device ID out of it
    return devices[GetDeviceID(URL)].first;
  }

  bool SupportsMultiplePrograms(const rdcstr &URL) override
  {
    // if this is false, UI will prevent launching program if one is detected to be running &
    // capturing
    return false;
  }

  bool IsSupported(const rdcstr &URL) override
  {
    // seems irrelevant, all devices supported? could check installed RenderDoc version is
    // compatible?
    return true;
  }

  ResultDetails StartRemoteServer(const rdcstr &URL) override
  {
    RDResult result = ResultCode::Succeeded;

    // TODO: future
    // this would be the opportunity to install the remote server bits (if we distribute them with
    // RD like android), and/or trigger the remote server to start on its own
    // we can block here if we want.

    return result;
  }

  rdcstr RemapHostname(const rdcstr &deviceID) override
  {
    // showing the example of assuming all devices are portforwarded from localhost
    // could return a real hostname for each if that's in the DB
    return "127.0.0.1";
  }

  uint16_t RemapPort(const rdcstr &deviceID, uint16_t srcPort) override
  {
    // handle whatever port forwarding scheme we use to keep port ranges distinct here
    // by default RD uses one for the remote server, and a range of up to 7 for tracking and
    // enumerating remote-controllable capturing programs
    uint16_t portbase = devices[deviceID].second;

    if(portbase == 0)
      return 0;

    // standard mapping of ports within range
    if(srcPort == RenderDoc_RemoteServerPort)
      return portbase + RenderDoc_ForwardRemoteServerOffset;
    else if(srcPort >= RenderDoc_FirstTargetControlPort && srcPort <= RenderDoc_LastTargetControlPort)
      return portbase + RenderDoc_ForwardTargetControlOffset;

    return 0;
  }

  IRemoteServer *CreateRemoteServer(Network::Socket *sock, const rdcstr &deviceID) override
  {
    // here we could override the RemoteServer class here to customise things - see other file in
    // gist
    return new SteamOSRemoteServer(sock, deviceID, RemapPort(deviceID, RenderDoc_RemoteServerPort));

    // default behaviour for everything - uses remote server connection to enumerate real filesystem on
    // the other side, asks the remote server to launch them via plain fork+execve on linux
    // return new RemoteServer(sock, deviceID);
  }

  static SteamOSController m_Inst;

  static IDeviceProtocolHandler *Get() { return &m_Inst; };
};

SteamOSController SteamOSController::m_Inst;

DeviceProtocolRegistration steamOSProtocol("steamos", &SteamOSController::Get);
