/* Orchid - WebRTC P2P VPN Market (on Ethereum)
 * Copyright (C) 2017-2019  The Orchid Authors
*/

/* GNU Affero General Public License, Version 3 {{{ */
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.

 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/
/* }}} */


#ifndef ORCHID_CLIENT_HPP
#define ORCHID_CLIENT_HPP

#include <atomic>

// XXX: give a patch to Lewis Baker to fix this
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreorder"
#include <cppcoro/shared_task.hpp>
#pragma clang diagnostic pop

#include <rtc_base/rtc_certificate.h>
#include <rtc_base/ssl_fingerprint.h>

#include "bond.hpp"
#include "crypto.hpp"
#include "endpoint.hpp"
#include "jsonrpc.hpp"
#include "locked.hpp"
#include "nest.hpp"
#include "origin.hpp"
#include "signed.hpp"
#include "ticket.hpp"

namespace orc {

class Client :
    public Pump<Buffer>,
    public Bonded
{
  private:
    const rtc::scoped_refptr<rtc::RTCCertificate> local_;

    const std::string url_;
    const U<rtc::SSLFingerprint> remote_;

    const Endpoint endpoint_;

    const Address lottery_;
    const uint256_t chain_;

    const Secret secret_;
    const Address funder_;

    const Address seller_;
    const Bytes shared_;

    const uint128_t face_;
    const uint256_t prepay_;

    struct Locked_ {
        uint64_t benefit_ = 0;
        std::map<Bytes32, std::pair<Ticket, Signature>> pending_;
        uint256_t spent_ = 0;

        int64_t serial_ = -1;
        checked_int256_t balance_ = 0;
        Bytes32 commit_ = Zero<32>();
        Address recipient_ = 0;
        cppcoro::shared_task<Bytes> ring_;
    }; Locked<Locked_> locked_;

    Nest nest_;
    Socket socket_;

    task<void> Submit();
    task<void> Submit(const Bytes32 &hash, const Ticket &ticket, const Bytes &receipt, const Signature &signature);

    void Issue(uint256_t amount);
    void Transfer(size_t size);

    cppcoro::shared_task<Bytes> Ring(Address recipient);

  protected:
    void Land(Pipe *pipe, const Buffer &data) override;
    void Stop() noexcept override;

  public:
    Client(BufferDrain &drain, std::string url, U<rtc::SSLFingerprint> remote, Endpoint endpoint, const Address &lottery, const uint256_t &chain, const Secret &secret, const Address &funder, const Address &seller, const uint128_t &face);
    ~Client() override;

    task<void> Open(const S<Origin> &origin);
    task<void> Shut() noexcept override;

    task<void> Send(const Buffer &data) override;

    void Update();
    uint256_t Spent();
    checked_int256_t Balance();
};

}

#endif//ORCHID_CLIENT_HPP
