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

#include "cyberlink.h"

#include <bcore/logmacro.h>
#include <boost/bind.hpp>

#include <bcore/os/sleep.h>
#include <bcore/t_string.h>
#include <bcore/verbose.h>
#include <daemon/modulelog.h>

namespace btg
{
   namespace daemon
   {
      namespace upnp
      {
         const std::string moduleName("upnp");

         const std::string gatewayDevice("urn:schemas-upnp-org:device:InternetGatewayDevice:1");
         const std::string wanDevice("urn:schemas-upnp-org:device:WANDevice:1");
         const std::string wanconDevice("urn:schemas-upnp-org:device:WANConnectionDevice:1");
         const std::string serviceType("urn:schemas-upnp-org:service:WANIPConnection:1");

         // Request to get external IP.
         const std::string externalIpReq("GetExternalIPAddress");
         const std::string newExternalIpReq("NewExternalIPAddress");

         // Timeout used for searching for devices. If no device is
         // discovered after 30 seconds, give up.
         const t_uint searchTimeout = 1024 * 30;

         cyberLinkCtrlPoint::cyberLinkCtrlPoint(btg::core::LogWrapperType _logwrapper,
                                                bool const _verboseFlag,
                                                btg::core::Address const& _addr)
            : btg::core::Logable(_logwrapper),
              verboseFlag_(_verboseFlag),
              addr_(_addr),
              deviceCount_(0),
              portsMapped_(false),
              service_(0)
         {
            addNotifyListener(this);
            addSearchResponseListener(this);
            addDeviceChangeListener(this);
         }

         bool cyberLinkCtrlPoint::portsMapped() const
         {
            return portsMapped_;
         }

         t_uint cyberLinkCtrlPoint::deviceCount() const
         {
            return deviceCount_;
         }

         bool cyberLinkCtrlPoint::init()
         {
            bool status = true;

            start(gatewayDevice.c_str());

            BTG_MNOTICE(logWrapper(), "cyberLinkCtrlPoint, search started.");

            BTG_MNOTICE(logWrapper(), "cyberLinkCtrlPoint, initialized.");

            return status;
         }

         bool cyberLinkCtrlPoint::deInit()
         {
            bool status = true;

            stop();

            return status;
         }

         bool cyberLinkCtrlPoint::search()
         {
            CyberLink::ControlPoint::search();

            return true;
         }

         void cyberLinkCtrlPoint::deviceNotifyReceived(CyberLink::SSDPPacket* _ssdpPacket)
         {
            
         }

         void cyberLinkCtrlPoint::deviceSearchResponseReceived(CyberLink::SSDPPacket* _ssdpPacket)
         {
            BTG_MNOTICE(logWrapper(), "cyberLinkCtrlPoint::deviceSearchResponseReceived.");

            if (_ssdpPacket->isDiscover())
               {
                  // Got at least one root device.
                  std::string usn;
                  std::string st;
                  std::string location;

                  _ssdpPacket->getUSN(usn);
                  _ssdpPacket->getST(st);
                  _ssdpPacket->getLocation(location);

                  BTG_MNOTICE(logWrapper(), "Root device found: usn = " << usn <<
                             ", st = " << st <<
                             ", location = " << location << ".");

               }
            /*
              std::string name = usn.substr( 0, usn.find( "::" ) );

              CyberLink::Device* device = getDevice(name.c_str());

              // Hopefully only gateway devices are discovered.

              CyberLink::ServiceList* services = device->getServiceList();
              CyberLink::ServiceList::const_iterator iter;
              for (iter = services->begin();
              iter != services->end();
              iter++)
              {
              // Yes, its true, the lib uses vectors of void pointers.
              // The horror, the horror!!!
              CyberLink::Service* service = static_cast<CyberLink::Service*>(*iter);

              std::string iservicetype(service->getServiceType());
              if (iservicetype == serviceType)
              {
              BTG_MNOTICE(logWrapper(), "Found an usable device/service.");
              }
              }
              if (device->getDeviceType() == "InternetGatewayDevice")
              {

              }
              }
            */
         }

         void cyberLinkCtrlPoint::deviceAdded(CyberLink::Device* _dev)
         {
            BTG_MNOTICE(logWrapper(), "Added device: " << _dev->getFriendlyName());

            deviceCount_++;
         }

         void cyberLinkCtrlPoint::deviceRemoved(CyberLink::Device* _dev)
         {
            BTG_MNOTICE(logWrapper(), "Removed device: " << _dev->getFriendlyName());

            std::string UDN(_dev->getUDN());

            if ((portsMapped_) && (deviceId_ == UDN))
               {
                  portsMapped_ = false;
                  service_     = 0;

                  BTG_MNOTICE(logWrapper(), "Device used for mapping was removed.");
               }

            deviceCount_--;
         }

         bool cyberLinkCtrlPoint::mapPorts(std::pair<t_int, t_int> const& _range)
         {
            // Get a list of root devices.

            CyberLink::DeviceList* rootdevices = CyberLink::ControlPoint::getDeviceList();

            t_uint numberOfDevices = rootdevices->size();

            if (numberOfDevices == 0)
               {
                  BTG_MNOTICE(logWrapper(), "No devices found.");
               }

            for (t_uint counter = 0;
                 counter < numberOfDevices;
                 counter++)
               {
                  CyberLink::Device* device = rootdevices->getDevice(counter);
                  if (mapPorts(device, _range))
                     {
                        return true;
                     }
               }

            VERBOSE_LOG(logWrapper(), verboseFlag_, "UPNP: Unable to map ports.");
            return false;
         }

         bool cyberLinkCtrlPoint::unMapPorts()
         {
            bool status = unMapPorts(udp_pml_) && unMapPorts(tcp_pml_);

            portsMapped_ = false;
            service_     = 0;

            return status;
         }

         bool cyberLinkCtrlPoint::unMapPorts(portMappingList const& _pml)
         {
            bool result = false;

            if (service_ == 0)
               {
                  return result;
               }

            CyberLink::Action* remove_action = service_->getAction("DeletePortMapping");

            if (remove_action == 0)
               {
                  BTG_MNOTICE(logWrapper(), "Port unmapping failed.");
                  return result;
               }

            result = true;

            std::vector<t_uint> const& mapping = _pml.mapping;

            for (std::vector<t_uint>::const_iterator iter = mapping.begin();
                 iter != mapping.end();
                 iter++)
               {
                  std::string port(btg::core::convertToString<t_uint>(*iter));

                  remove_action->setArgumentValue("NewRemoteHost", _pml.externalAddress.c_str());
                  remove_action->setArgumentValue("NewExternalPort", port.c_str());
                  std::string protocol;
                  if (_pml.protocol == portMappingList::TCP)
                     {
                        protocol = "TCP";
                     }
                  else
                     {
                        protocol = "UDP";
                     }

                  remove_action->setArgumentValue("NewProtocol", protocol.c_str());

                  bool status = remove_action->postControlAction();

                  if (!status)
                     {
                        BTG_MNOTICE(logWrapper(), "Unable to unmap " << protocol << " port: " << port << ".");
                        result = false;
                        break;
                     }
               }

            return result;
         }

         bool cyberLinkCtrlPoint::getExternalIp(CyberLink::Service* _service, std::string & _address)
         {
            return getIp(_service,
                         externalIpReq,
                         newExternalIpReq,
                         _address);
         }

         bool cyberLinkCtrlPoint::getIp(CyberLink::Service* _service,
                                        std::string const& _req1,
                                        std::string const& _req2,
                                        std::string & _address)
         {
            bool status = false;

            CyberLink::Action* ip = _service->getAction(_req1.c_str());

            if (ip == 0)
               {
                  BTG_MNOTICE(logWrapper(), "Unable to get action: " << _req1 << ".");
                  return status;
               }

            if (!ip->postControlAction())
               {
                  BTG_MNOTICE(logWrapper(), "postControlAction failed.");
                  return status;
               }

            CyberLink::Argument* ret = ip->getOutputArgumentList()->getArgument(_req2.c_str());

            if (ret == 0)
               {
                  BTG_MNOTICE(logWrapper(), "Unable to get argument " << _req2 << ".");
                  return status;
               }

            _address = ret->getValue();

            status = true;
            return status;
         }

         bool cyberLinkCtrlPoint::mapPorts(CyberLink::Device* _device, std::pair<t_int, t_int> const& _range)
         {
            // Find out if any embedded devices are useful.

            BTG_MNOTICE(logWrapper(), "Examining device: " << _device->getFriendlyName()
                       << ", type: " << _device->getDeviceType() << ".");

            if (_device->getDeviceType() == gatewayDevice)
               {
                  CyberLink::DeviceList* embdevices = _device->getDeviceList();

                  t_uint numberOfEmbDevices = embdevices->size();

                  for (t_uint counter = 0;
                       counter < numberOfEmbDevices;
                       counter++)
                     {
                        CyberLink::Device* device = embdevices->getDevice(counter);

                        BTG_MNOTICE(logWrapper(), "Examining embedded device(1): " << device->getFriendlyName()
                                   << ", type: " << device->getDeviceType() << ".");

                        if (device->getDeviceType() == wanDevice)
                           {
                              CyberLink::DeviceList* embdevices2 = device->getDeviceList();

                              t_uint numberOfDevices2 = embdevices2->size();

                              for (t_uint counter2 = 0;
                                   counter2 < numberOfDevices2;
                                   counter2++)
                                 {
                                    CyberLink::Device* device2 = embdevices2->getDevice(counter2);

                                    BTG_MNOTICE(logWrapper(), "Examining embedded device(2): " << device2->getFriendlyName()
                                               << ", type: " << device2->getDeviceType() << ".");

                                    if (device2->getDeviceType() == wanconDevice)
                                       {
                                          // Finally got a device that
                                          // can be used to map ports.
                                          BTG_MNOTICE(logWrapper(), "Attempting to map ports(2).");

                                          CyberLink::Service* service = device2->getService(serviceType.c_str());

                                          if (service != 0)
                                             {
                                                // Got a service.
                                                // 1. Get the external IP.
                                                std::string ext_address;
                                                if (getExternalIp(service, ext_address))
                                                   {
                                                      BTG_MNOTICE(logWrapper(), "External IP: " << ext_address);
                                                   }
                                                else
                                                   {
                                                      BTG_MNOTICE(logWrapper(), "Unable to obtain external IP.");
                                                   }

                                                BTG_MNOTICE(logWrapper(), "Internal address: " << addr_.toString());

                                                // 2. Map ports, both TCP and UDP.

                                                tcp_pml_ = portMappingList(_range,
                                                                           ext_address,
                                                                           addr_.toString(),
                                                                           portMappingList::TCP
                                                                           );

                                                bool tcp_result = mapPorts(service, tcp_pml_);

                                                udp_pml_ = tcp_pml_;

                                                udp_pml_.protocol = portMappingList::UDP;

                                                bool udp_result = mapPorts(service, udp_pml_);

                                                if (tcp_result && udp_result)
                                                   {
                                                      portsMapped_ = true;
                                                      // Save a textual id, which can
                                                      // be used to find this device again.

                                                      service_  = service;
                                                      deviceId_ = std::string(_device->getUDN());

                                                      BTG_MNOTICE(logWrapper(), "Ports mapped.");
                                                      return true;
                                                   }
                                             }
                                          else
                                             {
                                                BTG_MNOTICE(logWrapper(), "No service.");
                                             }
                                       }
                                 }
                           }
                     }
               }
            else
               {
                  BTG_MNOTICE(logWrapper(), "Not a WAN device.");
               }

            return false;
         }

         bool cyberLinkCtrlPoint::mapPorts(CyberLink::Service* _service, portMappingList const& _pml)
         {
            bool result = false;

            CyberLink::Action* add_action = _service->getAction("AddPortMapping");

            if (add_action == 0)
               {
                  BTG_MNOTICE(logWrapper(), "Port mapping failed.");
                  return result;
               }

            result = true;

            std::vector<t_uint> const& mapping = _pml.mapping;

            for (std::vector<t_uint>::const_iterator iter = mapping.begin();
                 iter != mapping.end();
                 iter++)
               {
                  std::string port(btg::core::convertToString<t_uint>(*iter));

                  add_action->setArgumentValue("NewRemoteHost", _pml.externalAddress.c_str());
                  add_action->setArgumentValue("NewExternalPort", port.c_str());
                  add_action->setArgumentValue("NewInternalClient", _pml.internalAddress.c_str());
                  add_action->setArgumentValue("NewInternalPort", port.c_str());
                  std::string protocol;
                  if (_pml.protocol == portMappingList::TCP)
                     {
                        protocol = "TCP";
                     }
                  else
                     {
                        protocol = "UDP";
                     }

                  add_action->setArgumentValue("NewProtocol", protocol.c_str());
                  add_action->setArgumentValue("NewPortMappingDescription", "test");
                  add_action->setArgumentValue("NewEnabled", "1");
                  add_action->setArgumentValue("NewLeaseDuration", 0);

                  bool status = add_action->postControlAction();

                  if (!status)
                     {
                        BTG_MNOTICE(logWrapper(), "Unable to map " << protocol << " port: " << port << ".");
                        result = false;
                        break;
                     }
               }

            return result;
         }


         cyberLinkCtrlPoint::~cyberLinkCtrlPoint()
         {

         }

         cyberlinkUpnpIf::cyberlinkUpnpIf(btg::core::LogWrapperType _logwrapper,
                                          bool const _verboseFlag,
                                          btg::core::Address const& _addr)
            : upnpIf(_logwrapper, _verboseFlag),
              addr_(_addr),
              die_(false),
              result_ready_(false),
              range_status_(false),
              count_(0),
              ctrl(_logwrapper, verboseFlag_, _addr)
         {
            BTG_MNOTICE(logWrapper(), "cyberlinkUpnpIf, constructed.");

            initialized_ = ctrl.init();

            if (!initialized_)
               {
                  BTG_MNOTICE(logWrapper(), "Unable to initialize cyberlink UPNP.");
                  return;
               }

            ctrl.search();
            
            pthread_.reset( new boost::thread( boost::bind(&cyberlinkUpnpIf::work, boost::ref(*this)) ) );
         }

         void cyberlinkUpnpIf::setAction(cyberlinkUpnpIf::ACTION _action)
         {
            boost::mutex::scoped_lock scoped_lock(interfaceMutex_);

            action_       = _action;
            result_ready_ = false;
         }

         bool cyberlinkUpnpIf::result_ready() const
         {
            return result_ready_;
         }

         bool cyberlinkUpnpIf::waitForDevice()
         {
            const t_int waitStep = 512;
            t_uint waitAmount    = 0;
            bool deviceFound     = false;

            while((waitAmount < searchTimeout) || deviceFound)
               {
                  {
                     boost::mutex::scoped_lock scoped_lock(resultMutex_);

                     setAction(cyberlinkUpnpIf::DEVCOUNT);

                     interfaceCondition_.wait(scoped_lock);

                     if (count_ >= 1)
                        {
                           BTG_MNOTICE(logWrapper(), "Found a device");

                           deviceFound = true;
                           break;
                        }
                     else
                        {
                           BTG_MNOTICE(logWrapper(), "Waiting for a device (" << waitAmount << ").");

                           waitAmount += waitStep;
                           btg::core::os::Sleep::sleepMiliSeconds(waitStep);
                        }
                  }
               }

            return deviceFound;
         }

         bool cyberlinkUpnpIf::open(std::pair<t_int, t_int> const& _range)
         {
            if (!waitForDevice())
               {
                  VERBOSE_LOG(logWrapper(), verboseFlag_, "UPnP: No UPnP devices discovered.");
                  BTG_MNOTICE(logWrapper(), "Waited, but no devices were discovered.");
                  return false;
               }

            boost::mutex::scoped_lock scoped_lock(resultMutex_);

            {
               boost::mutex::scoped_lock scoped_lock(interfaceMutex_);

               BTG_MNOTICE(logWrapper(), "Attempting to map ports(1).");

               action_ = cyberlinkUpnpIf::MAP;
               range_  = _range;
            }

            interfaceCondition_.wait(scoped_lock);

            return range_status_;
         }

         bool cyberlinkUpnpIf::close()
         {
            boost::mutex::scoped_lock scoped_lock(resultMutex_);

            {
               boost::mutex::scoped_lock scoped_lock(interfaceMutex_);

               BTG_MNOTICE(logWrapper(), "Attempting to unmap ports.");

               action_ = cyberlinkUpnpIf::UNMAP;
            }

            interfaceCondition_.wait(scoped_lock);

            return range_status_;
         }
         
         void cyberlinkUpnpIf::start_thread()
         {
            if (!die_)
               return;
            die_ = false;
            pthread_.reset( new boost::thread( boost::bind(&cyberlinkUpnpIf::work, boost::ref(*this)) ) ); 
         }
         
         void cyberlinkUpnpIf::stop_thread()
         {
            if (die_)
               return;
            die_ = true;
            pthread_->join();
         }

         void cyberlinkUpnpIf::work()
         {
            BTG_MNOTICE(logWrapper(), "cyberlinkUpnpIf, thread started.");

            if (!initialized_)
               {
                  BTG_MNOTICE(logWrapper(), "Unable to initialize cyberlink UPNP.");
                  return;
               }

            while(!die_)
               {
                  {
                     boost::mutex::scoped_lock scoped_lock(interfaceMutex_);

                     // React on interface calls.
                     switch(action_)
                        {
                        case cyberlinkUpnpIf::UNDEF:
                           {
                              btg::core::os::Sleep::sleepMiliSeconds(64);
                              break;
                           }
                        case cyberlinkUpnpIf::MAP:
                           {
                              BTG_MNOTICE(logWrapper(), "cyberlinkUpnpIf::MAP");

                              {
                                 boost::mutex::scoped_lock scoped_lock(resultMutex_);

                                 if (!ctrl.portsMapped())
                                    {
                                       range_status_ = ctrl.mapPorts(range_);
                                       result_ready_ = true;
                                    }
                                 else
                                    {
                                       // Lets not map the same ports
                                       // more than once.
                                       range_status_ = ctrl.portsMapped();
                                       result_ready_ = true;
                                    }
                                 interfaceCondition_.notify_one();

                                 action_ = cyberlinkUpnpIf::UNDEF;
                              }
                              break;
                           }
                        case cyberlinkUpnpIf::UNMAP:
                           {
                              {
                                 boost::mutex::scoped_lock scoped_lock(resultMutex_);

                                 if (ctrl.portsMapped())
                                    {
                                       BTG_MNOTICE(logWrapper(), "Unmapping ports.");
                                       range_status_ = ctrl.unMapPorts();
                                    }
                                 else
                                    {
                                       BTG_MNOTICE(logWrapper(), "Ports not mapped, so not unmapping.");
                                       range_status_ = false;
                                    }
                                 interfaceCondition_.notify_one();

                                 action_  = cyberlinkUpnpIf::UNDEF;
                              }
                              break;
                           }
                        case cyberlinkUpnpIf::DEVCOUNT:
                           {
                              BTG_MNOTICE(logWrapper(), "cyberlinkUpnpIf::DEVCOUNT");

                              {
                                 boost::mutex::scoped_lock scoped_lock(resultMutex_);

                                 count_        = ctrl.deviceCount();
                                 result_ready_ = true;

                                 interfaceCondition_.notify_one();

                                 action_  = cyberlinkUpnpIf::UNDEF;
                              }
                              break;
                           }
                        }
                  } // lock
               }

            BTG_MNOTICE(logWrapper(), "cyberlinkUpnpIf, thread finished.");
         }

         cyberlinkUpnpIf::~cyberlinkUpnpIf()
         {
            if (!die_)
            {
               die_ = true;
               pthread_->join();
            }

            if (initialized_)
            {
               // Before destructing, close a mapping, if one exists.
               if (ctrl.portsMapped())
                  {
                     BTG_MNOTICE(logWrapper(), "Unmapping ports.");
                     ctrl.unMapPorts();
                  }

               ctrl.deInit();
               initialized_ = false;

               BTG_MNOTICE(logWrapper(), "cyberlinkUpnpIf, destructed.");
            }
         }

      } // namespace upnp
   } // namespace daemon
} // namespace btg
