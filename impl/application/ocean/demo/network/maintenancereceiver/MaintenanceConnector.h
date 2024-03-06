// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_NETWORK_MAINTENANCE_RECEIVER_MAINTENANCE_CONNECTOR_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_NETWORK_MAINTENANCE_RECEIVER_MAINTENANCE_CONNECTOR_H

#include "application/ocean/demo/network/maintenancereceiver/ApplicationDemoNetworkMaintenanceReceiver.h"

/**
 * Starts the maintenance connector (the receiver of maintenance data).
 * @param useTCPConnector True, to use the TCP connector; False, to use the UDP connector
 * @param port The port number on which the connector listens
 * @return True, if succeeded
 * @ingroup applicationdemonetworkmaintenancereceiver
 */
bool startConnector(const bool useTCPConnector, const unsigned short port);

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_NETWORK_MAINTENANCE_RECEIVER_MAINTENANCE_CONNECTOR_H
