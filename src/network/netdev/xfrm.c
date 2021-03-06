/* SPDX-License-Identifier: LGPL-2.1+ */

#include "missing_network.h"
#include "netdev/xfrm.h"

static int xfrm_fill_message_create(NetDev *netdev, Link *link, sd_netlink_message *message) {
        int if_idx, r;
        Xfrm *x;

        assert(netdev);
        assert(message);

        x = XFRM(netdev);

        if (x->independent)
                if_idx = LOOPBACK_IFINDEX;
        else {
                assert(link);
                if (link->ifindex == 0)
                        return log_netdev_error_errno(netdev, SYNTHETIC_ERRNO(ENODEV), "Could not get interface index: %m");
                if_idx = link->ifindex;
        }

        r = sd_netlink_message_append_u32(message, IFLA_XFRM_LINK, if_idx);
        if (r < 0)
                return log_netdev_error_errno(netdev, r, "Could not append IFLA_XFRM_LINK: %m");

        r = sd_netlink_message_append_u32(message, IFLA_XFRM_IF_ID, x->if_id);
        if (r < 0)
                return log_netdev_error_errno(netdev, r, "Could not append IFLA_XFRM_IF_ID: %m");

        return 0;
}

const NetDevVTable xfrm_vtable = {
        .object_size = sizeof(Xfrm),
        .sections = "Match\0NetDev\0Xfrm\0",
        .fill_message_create = xfrm_fill_message_create,
        .create_type = NETDEV_CREATE_STACKED
};
