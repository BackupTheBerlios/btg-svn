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

#include "levelone.h"

#include <bcore/logmacro.h>
#include <bcore/util.h>

#include <daemon/lt_version.h>
#include <daemon/modulelog.h>

#include <fstream>

#include <asio/ip/address_v4.hpp>

#include <bcore/verbose.h>

namespace btg
{
   namespace daemon
   {
      const std::string moduleName("flt");

      levelOne::levelOne(btg::core::LogWrapperType _logwrapper,
                         bool const _verboseFlag,
                         std::string const& _filename)
         : IpFilterIf(_logwrapper,
                      _verboseFlag,
                      IpFilterIf::IPF_LEVELONE,
                      _filename)
      {
         using namespace btg::core;

         std::ifstream file;

#if HAVE_IOS_BASE
         file.open(filename_.c_str(), std::ios_base::in);
#else
         file.open(filename_.c_str(), std::ios::in);
#endif

         BTG_MNOTICE(logWrapper(), getName() << ": attempting to open '" << filename_ << "'");

         if (file.is_open())
            {
               std::string input;
               t_strList descr_range;
               t_strList IPs;
               t_strList str_ip_ip0;
               t_strList str_ip_ip1;
               t_uint ipa0[4] = {0};
               t_uint ipa1[4] = {0};

               bool failed = false;

               while (!file.eof())
                  {
                     getline(file, input);

                     if (input.size() > 0)
                        {
                           descr_range = Util::splitLine(input, ":");

                           if (descr_range.size() == 2)
                              {
                                 IPs = Util::splitLine(descr_range[1], "-");
                                 if (IPs.size() == 2)
                                    {
                                       str_ip_ip0 = Util::splitLine(IPs[0], ".");
                                       str_ip_ip1 = Util::splitLine(IPs[1], ".");

                                       if ((str_ip_ip0.size() >= 4) &&
                                           (str_ip_ip1.size() >= 4))
                                          {
                                             failed = false;

                                             t_int ip_part;
                                             for (t_uint counter = 0; counter<4; counter++)
                                                {
                                                   ip_part = -1;
                                                   btg::core::stringToNumber<t_int>(str_ip_ip0[counter], ip_part);

                                                   if ((ip_part >= 0) && (ip_part <= 255))
                                                      {
                                                         ipa0[counter] = ip_part;
                                                      }
                                                   else
                                                      {
                                                         failed = true;
                                                         break;
                                                      }

                                                   ip_part = -1;
                                                   btg::core::stringToNumber<t_int>(str_ip_ip1[counter], ip_part);

                                                   if ((ip_part >= 0) && (ip_part <= 255))
                                                      {
                                                         ipa1[counter] = ip_part;
                                                      }
                                                   else
                                                      {
                                                         failed = true;
                                                         break;
                                                      }
                                                }

                                             if (!failed)
                                                {
                                                   boost::array<unsigned char, 4u> const ip1 = {{ipa0[0], ipa0[1], ipa0[2], ipa0[3]}};
                                                   boost::array<unsigned char, 4u> const ip2 = {{ipa1[0], ipa1[1], ipa1[2], ipa1[3]}};
                                                   asio::ip::address_v4 address1(ip1);
                                                   asio::ip::address_v4 address2(ip2);
                                                   
                                                   filter_.add_rule(address1,
                                                                    address2,
                                                                    libtorrent::ip_filter::blocked);
                                                   numberOfentries_++;

                                                   if ((numberOfentries_ % 1000) == 0)
                                                      {
                                                         VERBOSE_LOG(logWrapper(), verboseFlag_, "Filter " << getName() << " added " << numberOfentries_ << " entries.");
                                                      }
                                                }
                                          }
                                    }
                              }
                        }
                  }

               file.close();

               if (numberOfentries_ > 0)
                  {
                     initialized_ = true;
                     BTG_MNOTICE(logWrapper(), getName() << ": initialized, " << numberOfentries_ << " entries");
                  }
               else
                  {
                     BTG_MNOTICE(logWrapper(), getName() << ": not initialized");
                  }
            }
         else
            {
               BTG_MNOTICE(logWrapper(), getName() << ": unable to open file: '" << filename_ << "'");
            }
      }

      void levelOne::set(libtorrent::session & _session) const
      {
         BTG_MNOTICE(logWrapper(), getName() << ": making session use filter");
         _session.set_ip_filter(filter_);
      }

      std::string levelOne::getName() const
      {
         return "IPF_LEVELONE";
      }

      levelOne::~levelOne()
      {
      }

   } // namespace daemon
} // namespace btg



