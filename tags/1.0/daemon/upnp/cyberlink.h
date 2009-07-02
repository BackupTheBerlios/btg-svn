/*
 * btg Copyright (C) 2005 Michael Wojciechowski.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * $Id$
 */

#ifndef UPNP_CYBERLINK_H
#define UPNP_CYBERLINK_H

#include "upnpif.h"
#include "portmap.h"
#include <bcore/addrport.h>
#include <bcore/logable.h>

#include <cybergarage/upnp/ControlPoint.h>
#include <cybergarage/upnp/device/NotifyListener.h>
#include <cybergarage/upnp/device/SearchResponseListener.h>
#include <cybergarage/upnp/device/DeviceChangeListener.h>
#include <cybergarage/upnp/ssdp/SSDPPacket.h>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

namespace btg
{
   namespace daemon
      {
         namespace upnp
            {
               /**
                * \addtogroup upnp
                */
               /** @{ */

               /// Control point using the CyberLink UPnP library.
               class cyberLinkCtrlPoint:
                  public CyberLink::ControlPoint,
                  public CyberLink::NotifyListener,
                  public CyberLink::SearchResponseListener,
                  public CyberLink::DeviceChangeListener,
                  public btg::core::Logable
                  {
                  public:
                     /// Constructor.
                     cyberLinkCtrlPoint(btg::core::LogWrapperType _logwrapper,
                                        bool const _verboseFlag,
                                        btg::core::Address const& _addr);

                     /// Initialize.
                     bool init();

                     /// Uninitialize.
                     bool deInit();

                     /// Search for UPnP devices.
                     bool search();

                     /// Forward ports.
                     /// @param [in] _range The port range to use.
                     bool mapPorts(std::pair<t_int, t_int> const& _range);

                     /// Remove already forwarded ports.
                     bool unMapPorts();

                     /// Get a number of found devices.
                     /// Used to determine if enough devices was
                     /// found, so that an attempt can be made at
                     /// forwarding ports.
                     t_uint deviceCount() const;

                     /// Indicates if ports are already mapped.
                     /// @return True - ports are mapped. False otherwise.
                     bool portsMapped() const;

                     /// Destructor.
                     virtual ~cyberLinkCtrlPoint();

                  private:
                     /// Map ports, if a device was found.
                     bool mapPorts(CyberLink::Device* _device, std::pair<t_int, t_int> const& _range);
                     /// Map ports, if a service was found.
                     bool mapPorts(CyberLink::Service* _service, portMappingList const& _pml);

                     /// Remove port mapping.
                     bool unMapPorts(portMappingList const& _pml);

                     /// Get the external IP address of a service.
                     /// This would be the external IP of a NAT or firewall.
                     bool getExternalIp(CyberLink::Service* _service, std::string & _address);

                     /// Generic function, used by getExternalIp.
                     bool getIp(CyberLink::Service* _service,
                                std::string const& _req1,
                                std::string const& _req2,
                                std::string & _address);

                     /// Not used at the moment.
                     virtual void deviceNotifyReceived(CyberLink::SSDPPacket* ssdpPacket);

                     /// A device was discovered.
                     virtual void deviceSearchResponseReceived(CyberLink::SSDPPacket* ssdpPacket);

                     /// Cyberlink added a device.
                     virtual void deviceAdded(CyberLink::Device* _dev);

                     /// Cyberlink removed a device.
                     virtual void deviceRemoved(CyberLink::Device* _dev);

                  private:
                     /// Indicates if this interface should do verbose
                     /// logging.
                     bool const          verboseFlag_;

                     /// The internal address used.
                     btg::core::Address  addr_;

                     /// The number of devices this control point
                     /// knows about.
                     t_uint              deviceCount_;

                     /// Indicates if ports are already mapped.
                     bool                portsMapped_;

                     /// List of TCP ports mapped or to be mapped.
                     portMappingList     tcp_pml_;

                     /// List of UDP ports mapped or to be mapped.
                     portMappingList     udp_pml_;

                     /// UDN for the first device. Used for checking
                     /// when devices are removed.
                     std::string         deviceId_;

                     /// Pointer to the service used for mapping
                     /// ports. Unless the device it belongs to is
                     /// removed, it will be used to unmap forwarded
                     /// ports.
                     CyberLink::Service* service_;
                     
                     /// mapping of UUID to local addres, which (local addr.) is seen by Device.
                     /// Filled in deviceSearchResponseReceived
                     /// Used by mapPorts
                     std::map<std::string /* UUID */, btg::core::Address /* local IP */ > UUID2localAddr_;
                  };

               /// Upnp interface using the CyberLink UPnP library.
               /// This interface uses one boost thread and various
               /// synchorization methods to block on function calls. 
               /// Most of the Cyberlink library is non-blocking.
               class cyberlinkUpnpIf: public upnpIf
                  {
                  public:
                     /// Constructor.
                     cyberlinkUpnpIf(btg::core::LogWrapperType _logwrapper,
                                     bool const _verboseFlag,
                                     btg::core::Address const& _addr);

                     bool open(std::pair<t_int, t_int> const& _range);

                     bool close();
                     
                     void suspend();

                     void resume();

                     /// Destructor.
                     virtual ~cyberlinkUpnpIf();

                  private:
                     /// The internal IP address used.
                     btg::core::Address addr_;

                     /// Actions.
                     enum ACTION
                        {
                           UNDEF = 0, //!< Undefined.
                           MAP,       //!< Map ports.
                           UNMAP,     //!< Unmap ports.
                           DEVCOUNT   //!< Count avaiable devices.
                        };

                     /// What action the thread should execute next.
                     ACTION         action_;

                     /// Mutex used to control access to the members
                     /// of this class from the outside.
                     boost::mutex     interfaceMutex_;

                     /// Mutex used for communication of results from
                     /// an action executed by the thread.
                     boost::mutex     resultMutex_;

                     /// Condition variable used for communication of
                     /// results from an action executed by the
                     /// thread.
                     boost::condition interfaceCondition_;

                     /// True if the thread should terminate itself.
                     bool             die_;

                     /// Indicates if the result of an action is ready.
                     bool             result_ready_;

                     /// The thread used by this interface.
                     std::auto_ptr<boost::thread>    pthread_;

                     /// Port range.
                     std::pair<t_int, t_int> range_;

                     /// Status of the port range.
                     bool                    range_status_;

                     /// The number of devices.
                     t_uint                  count_;
                  private:
                     /// Function used for the thread.
                     void work();

                     /// Select an action for the thread to execute.
                     void setAction(cyberlinkUpnpIf::ACTION _action);

                     /// Indicates if a result is ready.
                     bool result_ready() const;

                     /// Wait for at least one device to become available.
                     bool waitForDevice();
                     
                     /// ctl point
                     cyberLinkCtrlPoint ctrl;
                  };

               /** @} */

            } // namespace upnp
      } // namespace daemon
} // namespace btg

#endif // UPNP_CYBERLINK_H
