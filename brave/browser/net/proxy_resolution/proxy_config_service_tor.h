// Copyright (c) 2018 The Brave Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BRAVE_BROWSER_NET_PROXY_RESOLUTION_PROXY_CONFIG_SERVICE_TOR_H_
#define BRAVE_BROWSER_NET_PROXY_RESOLUTION_PROXY_CONFIG_SERVICE_TOR_H_

#include <string>
#include <map>
#include <utility>

#include "base/compiler_specific.h"
#include "base/timer/timer.h"
#include "net/base/net_errors.h"
#include "net/base/net_export.h"
#include "net/proxy_resolution/proxy_config.h"
#include "net/proxy_resolution/proxy_config_service.h"
#include "vendor/brightray/browser/url_request_context_getter.h"

namespace base {
class Time;
}

namespace net {

const char kSocksProxy[] = "socks5";

// Implementation of ProxyConfigService that returns a tor specific result.
class NET_EXPORT ProxyConfigServiceTor : public ProxyConfigService {
 public:
  // Used to cache <username, password> of proxies
  struct TorProxyMap {
    std::map<std::string, std::pair<std::string, base::Time> > map;
    std::priority_queue<std::pair<base::Time, std::string> > queue;
  };

  explicit ProxyConfigServiceTor(const std::string& tor_proxy,
                                 const std::string& username,
                                 TorProxyMap* map);
  ~ProxyConfigServiceTor() override;

  static void TorSetProxy(
    net::ProxyResolutionService* service,
    const std::string& tor_proxy,
    const std::string& site_url,
    TorProxyMap* tor_proxy_map,
    bool new_password);

  // ProxyConfigService methods:
  void AddObserver(Observer* observer) override {}
  void RemoveObserver(Observer* observer) override {}
  ConfigAvailability GetLatestProxyConfig(
    ProxyConfigWithAnnotation* config) override;

 private:
  // Generate a new 128 bit random tag
  std::string GenerateNewPassword();

  void ClearExpiredEntries();

  ProxyConfig config_;

  std::string scheme_;
  std::string host_;
  std::string port_;

  TorProxyMap* tor_proxy_map_;
  base::OneShotTimer timer_;
};

}  // namespace net

#endif  // BRAVE_BROWSER_NET_PROXY_RESOLUTION_PROXY_CONFIG_SERVICE_TOR_H_
