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
 * $Id: trackerstatus.h,v 1.1.2.11 2007/02/14 22:11:47 wojci Exp $
 */

#ifndef TRACKERSTATUS_H
#define TRACKERSTATUS_H

#include <bcore/serializable.h>
#include <bcore/printable.h>

#include <string>

namespace btg
{
   namespace core
      {
         /**
          * \addtogroup core
          */
         /** @{ */

         /// Used for transfering status information about torrent-contexts.
         class trackerStatus: public Serializable, public Printable
            {
            public:

               /// Represents codes which were returned by the tracker.
	       /// 
	       /// The codes are both libtorrent specific codes and
	       /// HTTP error codes.

               enum tracker_status
                  {
                     /// Undefined.
                     undefined                              = -1,

		     /// Tracker timeout.
		     time_out                               = 0,

		     /// Tracker warning.
		     /// 
		     /// Adds a string which describes the warning.
		     warning                                = 1,

                     /// Info: Continue.
                     info_continue                          = 100,
                     /// Info: Switching Protocols.
                     info_switching_protocols               = 101,

                     /// Successful: OK.
                     succeeded_ok                           = 200,
                     /// Successful: Created.
                     succeeded_created                      = 201,
                     /// Successful: Accepted.
                     succeeded_accepted                     = 202,
                     /// Successful: Non-Authoritative Information.
                     succeeded_non_authorative_info         = 203,
                     /// Successful: No Content.
                     succeeded_no_content                   = 204,
                     /// Successful: Reset Content.
                     succeeded_reset_content                = 205,
                     /// Successful: Partial Content.
                     succeeded_partial_content              = 206,

                     /// Redirection: Multiple Choices.
                     redirection_multiple_choices           = 300,
                     /// Redirection: Moved Permanently.
                     redirection_moved_permanently          = 301,
                     /// Redirection: Found.
                     redirection_found                      = 302,
                     /// Redirection: See Other.
                     redirection_see_other                  = 303,
                     /// Redirection: Not Modified.
                     redirection_not_modified               = 304,
                     /// Redirection: Use Proxy.
                     redirection_use_proxy                  = 305,
                     /// Redirection: (Unused).
                     redirection_unused                     = 306,
                     /// Redirection: Temporary Redirect.
                     redirection_temporary_redirect         = 307,

                     /// Client Error: Bad Request.
                     client_bad_request                     = 400,
                     /// Client Error: Unauthorized.
                     client_unauthorized                    = 401,
                     /// Client Error: Payment Required.
                     client_payment_required                = 402,
                     /// Client Error: Forbidden.
                     client_forbidden                       = 403,
                     /// Client Error: Not Found.
                     client_not_found                       = 404,
                     /// Client Error: Method Not Allowed.
                     client_method_not_allowed              = 405,
                     /// Client Error: Not Acceptable.
                     client_not_acceptable                  = 406,
                     /// Client Error: Proxy Authentication Required.
                     client_proxy_authentication_required   = 407,
                     /// Client Error: Request Timeout.
                     client_request_timeout                 = 408,
                     /// Client Error: Conflict.
                     client_conflict                        = 409,
                     /// Client Error: Gone.
                     client_gone                            = 410,
                     /// Client Error: Length Required.
                     client_length_required                 = 411,
                     /// Client Error: Precondition Failed.
                     client_precondition_failed             = 412,
                     /// Client Error: Request Entity Too Large.
                     client_request_entity_too_large        = 413,
                     /// Client Error: Request-URI Too Long.
                     client_request_uri_too_long            = 414,
                     /// Client Error: Unsupported Media Type.
                     client_unsupported_media_type          = 415,
                     /// Client Error: Requested Range Not Satisfiable.
                     client_requested_range_not_satisfiable = 416,
                     /// Client Error: Expectation Failed.
                     client_expectation_failed              = 417,

                     /// Server Error: Internal Server Error.
                     server_internal_server_error           = 500,
                     /// Server Error: Not Implemented.
                     server_not_implemented                 = 501,
                     /// Server Error: Bad Gateway.
                     server_bad_gateway                     = 502,
                     /// Server Error: Service Unavailable.
                     server_service_unavailable             = 503,
                     /// Server Error: Gateway Timeout.
                     server_gateway_timeout                 = 504,
                     /// Server Error: HTTP Version Not Supported.
                     server_http_version_not_supported      = 505,

                     /// Internal Error: Malformed URI.
                     server_internal_malformed_uri          = 600,
                     /// Internal Error: Connection Timed Out.
                     server_internal_connection_timed_out   = 601,
                     /// Internal Error: Unknown Error.
                     server_internal_unknown_error          = 602,
                     /// Internal Error: Could Not Parse Reply.
                     server_internal_could_not_parse_reply  = 603,
                     /// Internal Error: Protocol Not Supported.
                     server_internal_protocol_not_supported = 604
                  };

	       enum
		 {
		   MIN_SERIAL = 0,   //!< Min serial value.
		   MAX_SERIAL = 1024 //!< Max serial value.
		 };

               /// Default constructor.
               /// Creates an undefined status.
               trackerStatus();

               /// Constructor.
               /// @param [in] _serial      Serial of this status.
               /// @param [in] _httpStatus  HTTP code.
               trackerStatus(t_int const _serial, t_int const _httpStatus);

               /// Copy constructor.
               /// @param [in] _trackerstatus Reference to the object to copy.
               trackerStatus(trackerStatus const& _trackerstatus);

               std::string toString() const;

               /// Get a textual description of the contained status
               /// code.
               std::string getDescription() const;

               /// The equality operator.
               bool operator== (trackerStatus const& _trackerstatus) const;

	       /// The not equal operator.
               bool operator!= (trackerStatus const& _trackerstatus) const;

               /// The assignment operator.
               trackerStatus& operator= (trackerStatus const& _trackerstatus);

               /// Get the status.
               tracker_status status()   const { return status_; }

               /// Return the contained serial.
               t_int serial() const { return serial_; }

               /// Used to convert this object to a byte array. This does not
               /// create a valid representation, since the session information
               /// and command type is not included, only the data from this
               /// object.
               bool serialize(btg::core::externalization::Externalization* _e) const;

               /// Setup this object with data contained in the byte array.
               bool deserialize(btg::core::externalization::Externalization* _e);

               /// Mark an trackerstatus as undefined.
               void invalidate();

	       /// Set the message describing this status.
	       void setMessage(std::string const& _message);

	       /// Get the message describing this status.
	       /// @param [in] _message Reference to a string to which
	       /// the message will be copied to.
	       /// @return True - message is present. False otherwise.
	       bool getMessage(std::string & _message);

               /// Destructor.
               ~trackerStatus();

               /// Set the status.
               void setStatus(t_int const _httpStatus);

               /// Set the serial.
               void setSerial(t_int const _serial);
            private:
               /// Return true if the contained HTTP code is valid.
               bool statusOk(t_int const _httpStatus) const;

               /// HTTP code.
               tracker_status status_;

               /// Serial.
               t_int          serial_;

	       /// The contained message.
	       std::string    message_;

	       /// Indicates if the contained message is set.
	       bool           message_set_;
            };

         /** @} */

      } // namespace core
} // namespace btg

#endif
