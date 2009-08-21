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

#include "trackerstatus.h"

#include <bcore/t_string.h>
#include <bcore/hru.h>
#include <bcore/hrr.h>

#include <bcore/helpermacro.h>
#include <bcore/t_string.h>

namespace btg
{
   namespace core
   {

      trackerStatus::trackerStatus()
         : Printable(),
           status_(undefined),
           serial_(-1),
           message_(),
           message_set_(false)
      {
         setStatus(trackerStatus::undefined);
      }

      trackerStatus::trackerStatus(t_int const _serial, t_int const _httpStatus)
         : Printable(),
           status_(undefined),
           serial_(_serial),
           message_(),
           message_set_(false)
      {
         setStatus(_httpStatus);
      }

      trackerStatus::trackerStatus(trackerStatus const& _status)
         : Serializable(),
           Printable(),
           status_(_status.status_),
           serial_(_status.serial_),
           message_(_status.message_),
           message_set_(_status.message_set_)
      {
      }

      void trackerStatus::setStatus(t_int const _httpStatus)
      {
         if (statusOk(_httpStatus))
            {
               status_ = static_cast<tracker_status>(_httpStatus);
            }
         else
            {
               status_ = undefined;
            }
      }

      void trackerStatus::setSerial(t_int const _serial)
      {
         serial_ = _serial;
      }

      bool trackerStatus::statusOk(t_int const _httpStatus) const
      {
         bool result = false;

         switch(_httpStatus)
            {
            case undefined:
            case time_out:
            case warning:
            case info_continue:
            case info_switching_protocols:
            case succeeded_ok:
            case succeeded_created:
            case succeeded_accepted:
            case succeeded_non_authorative_info:
            case succeeded_no_content:
            case succeeded_reset_content:
            case succeeded_partial_content:
            case redirection_multiple_choices:
            case redirection_moved_permanently:
            case redirection_found:
            case redirection_see_other:
            case redirection_not_modified:
            case redirection_use_proxy:
            case redirection_unused:
            case redirection_temporary_redirect:
            case client_bad_request:
            case client_unauthorized:
            case client_payment_required:
            case client_forbidden:
            case client_not_found:
            case client_method_not_allowed:
            case client_not_acceptable:
            case client_proxy_authentication_required:
            case client_request_timeout:
            case client_conflict:
            case client_gone:
            case client_length_required:
            case client_precondition_failed:
            case client_request_entity_too_large:
            case client_request_uri_too_long:
            case client_unsupported_media_type:
            case client_requested_range_not_satisfiable:
            case client_expectation_failed:
            case server_internal_server_error:
            case server_not_implemented:
            case server_bad_gateway:
            case server_service_unavailable:
            case server_gateway_timeout:
            case server_http_version_not_supported:
            case server_internal_malformed_uri:
            case server_internal_connection_timed_out:
            case server_internal_unknown_error:
            case server_internal_could_not_parse_reply:
            case server_internal_protocol_not_supported:
               {
                  result = true;
                  break;
               }
            default:
               {
                  result = false;
               }
            }
         return result;
      }

      bool trackerStatus::serialize(btg::core::externalization::Externalization* _e) const
      {
         // int, status;
         t_int temp_status  = status_;
         BTG_RCHECK( _e->intToBytes(&temp_status) );

         // int, serial;
         BTG_RCHECK( _e->intToBytes(&serial_) );

         // Optional message.
         BTG_RCHECK( _e->boolToBytes(message_set_) );
         if (message_set_)
            {
               BTG_RCHECK( _e->stringToBytes(&message_) );
            }

         return true;
      }

      bool trackerStatus::deserialize(btg::core::externalization::Externalization* _e)
      {
         // int, status;
         t_int temp_status  = 0;
         BTG_RCHECK( _e->bytesToInt(&temp_status) );

         // Check that the input integer is a status.
         if (!statusOk(temp_status))
            {
               BTG_RCHECK(false);
            }

         status_ = static_cast<tracker_status>(temp_status);

         BTG_RCHECK( _e->bytesToInt(&serial_) );

         BTG_RCHECK( _e->bytesToBool(message_set_) );

         if (message_set_)
            {
               BTG_RCHECK( _e->bytesToString(&message_) );
            }

         return true;
      }

      std::string trackerStatus::toString() const
      {
         std::string output("");

         output += "status (" + convertToString<t_int>(serial_) + "): " + getDescription();

         return output;
      }

      std::string trackerStatus::getDescription() const
      {
         std::string output;

         switch(status_)
            {
            case time_out:
               output = "Time out";
               break;
            case warning:
               output = "Warning";
               break;
            case info_continue:
               output = "Continue";
               break;
            case info_switching_protocols:
               output = "Switching protocols";
               break;
            case succeeded_ok:
               output = "OK";
               break;
            case succeeded_created:
               output = "Created";
               break;
            case succeeded_accepted:
               output = "Accepted";
               break;
            case succeeded_non_authorative_info:
               output = "Non-Authorative Information";
               break;
            case succeeded_no_content:
               output = "No Content";
               break;
            case succeeded_reset_content:
               output = "Reset Content";
               break;
            case succeeded_partial_content:
               output = "Partial Content";
               break;
            case redirection_multiple_choices:
               output = "Multiple Choices";
               break;
            case redirection_moved_permanently:
               output = "Moved Permanently";
               break;
            case redirection_found:
               output = "Found";
               break;
            case redirection_see_other:
               output = "See Other";
               break;
            case redirection_not_modified:
               output = "Not Modified";
               break;
            case redirection_use_proxy:
               output = "Use Proxy";
               break;
            case redirection_unused:
               output = "Unused";
               break;
            case redirection_temporary_redirect:
               output = "Temporary Redirect";
               break;
            case client_bad_request:
               output = "Bad Request";
               break;
            case client_unauthorized:
               output = "Unauthorized";
               break;
            case client_payment_required:
               output = "Payment Required";
               break;
            case client_forbidden:
               output = "Forbidden";
               break;
            case client_not_found:
               output = "Not Found";
               break;
            case client_method_not_allowed:
               output = "Method Not Allowed";
               break;
            case client_not_acceptable:
               output = "Not Acceptable";
               break;
            case client_proxy_authentication_required:
               output = "Proxy Authentication Required";
               break;
            case client_request_timeout:
               output = "Request Timeout";
               break;
            case client_conflict:
               output = "Conflict";
               break;
            case client_gone:
               output = "Gone";
               break;
            case client_length_required:
               output = "Length Required";
               break;
            case client_precondition_failed:
               output = "Precondition Failed";
               break;
            case client_request_entity_too_large:
               output = "Request Entity Too Large";
               break;
            case client_request_uri_too_long:
               output = "Request-URI Too Large";
               break;
            case client_unsupported_media_type:
               output = "Unsupported Media Type";
               break;
            case client_requested_range_not_satisfiable:
               output = "Requested Range Not Satisfiable";
               break;
            case client_expectation_failed:
               output = "Expectation Failed";
               break;
            case server_internal_server_error:
               output = "Internal Server Error";
               break;
            case server_not_implemented:
               output = "Not Implemented";
               break;
            case server_bad_gateway:
               output = "Bad Gateway";
               break;
            case server_service_unavailable:
               output = "Service Unavailable";
               break;
            case server_gateway_timeout:
               output = "Gateway Timeout";
               break;
            case server_http_version_not_supported:
               output = "HTTP Version not supported";
               break;
            case server_internal_malformed_uri:
               output = "Malformed URI";
               break;
            case server_internal_connection_timed_out:
               output = "Connection Timed Out";
               break;
            case server_internal_unknown_error:
               output = "Unknown Error";
               break;
            case server_internal_could_not_parse_reply:
               output = "Could Not Parse Reply";
               break;
            case server_internal_protocol_not_supported:
               output = "Protocol Not Supported";
               break;
            default:
               output = "Undefined Error";
               break;
            }

         output += " (" + convertToString<t_int>(status_) + ")";

         // If a message was set, append it here.
         if (message_set_)
            {
               output += ": " + message_;
            }

         return output;
      }

      bool trackerStatus::operator== (trackerStatus const& _status) const
      {
         bool op_status = true;

         if ((status_ != _status.status_) || 
             (serial_ != _status.serial_) || 
             (message_set_ != _status.message_set_) || 
             (message_ != _status.message_))
            {
               op_status = false;
            }

         return op_status;
      }

      bool trackerStatus::operator!= (trackerStatus const& _trackerstatus) const
      {
         return !(*this == _trackerstatus);
      }

      trackerStatus& trackerStatus::operator= (trackerStatus const& _status)
      {
         bool eq = (_status == *this);

         if (!eq)
            {
               status_      = _status.status_;
               serial_      = _status.serial_;
               message_set_ = _status.message_set_;
               message_     = _status.message_;
            }

         return *this;
      }

      void trackerStatus::invalidate()
      {
         setStatus(trackerStatus::undefined);
         setSerial(-1);
         message_set_ = false;
         message_.clear();
      }

      void trackerStatus::setMessage(std::string const& _message)
      {
         message_set_ = true;
         message_     = _message;
      }

      bool trackerStatus::getMessage(std::string & _message)
      {
         if (message_set_)
            {
               _message = message_;
            }

         return message_set_;
      }

      trackerStatus::~trackerStatus()
      {}

   } // namespace core
} // namespace btg
