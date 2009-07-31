#include "session_list.h"
#include <bcore/verbose.h>

#include "modulelog.h"
#include <daemon/ipfilter/ipfilterif.h>

namespace btg
{
   namespace daemon
   {
      const std::string moduleName("lst");

      sessionList::sessionList(btg::core::LogWrapperType _logwrapper,
                               bool const _verboseFlag,
                               t_uint const _maxSessions)
         : btg::core::Logable(_logwrapper),
           verboseFlag_(_verboseFlag),
           maxSessions_(_maxSessions),
           current_id_(1),
           eventhandlers_()
      {

      }

      void sessionList::new_session(t_long & _new_session)
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         bool unique = false;

         while (!unique)
            {
               get_session(_new_session);
               if (check_session(_new_session))
                  {
                     unique = true;
                  }
            }
      }

      void sessionList::get_session(t_long & _new_session)
      {
         _new_session = current_id_;

         if ((current_id_ + 1) < maxSessions_)
            {
               current_id_++;
            }
         else
            {
               current_id_ = 1;
            }
      }

      bool sessionList::check_session(t_long const _session) const
      {
         std::map<t_long, eventHandler*>::const_iterator iter = eventhandlers_.find(_session);

         if (iter == eventhandlers_.end())
            {
               return true;
            }

         return false;
      }

      void sessionList::add(t_long const _session, 
                            eventHandler* _eventhandler, 
                            bool _resetCounter)
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         std::pair<t_long, eventHandler*> p(_session, _eventhandler);

         eventhandlers_.insert(p);

         if (_resetCounter)
            {
               current_id_ = _session;
            }
      }

      bool sessionList::get(t_long const & _session, 
                            eventHandler* & _eventhandler)
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         std::map<t_long, eventHandler*>::const_iterator iter = eventhandlers_.find(_session);

         if (iter == eventhandlers_.end())
            {
               return false;
            }

         _eventhandler = iter->second;
         return true;
      }

      void sessionList::getIds(std::vector<t_long> & _sessions)
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         std::map<t_long, eventHandler*>::const_iterator iter;
         for (iter = eventhandlers_.begin();
              iter != eventhandlers_.end();
              iter++)
            {
               _sessions.push_back(iter->first);
            }
      }

      void sessionList::getIds(std::string const& _username, 
                               std::vector<t_long> & _sessions)
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         std::map<t_long, eventHandler*>::const_iterator iter;
         for (iter = eventhandlers_.begin();
              iter != eventhandlers_.end();
              iter++)
            {
               if (iter->second->getUsername() == _username)
                  {
                     _sessions.push_back(iter->first);
                  }
            }
      }

      void sessionList::getNames(std::string const& _username, 
                                 std::vector<std::string> & _names)
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         std::map<t_long, eventHandler*>::const_iterator iter;
         for (iter = eventhandlers_.begin();
              iter != eventhandlers_.end();
              iter++)
            {
               if (iter->second->getUsername() == _username)
                  {
                     _names.push_back(iter->second->getName());
                  }
            }
      }

      void sessionList::getNames(std::vector<std::string> & _names)
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         std::map<t_long, eventHandler*>::const_iterator iter;
         for (iter = eventhandlers_.begin();
              iter != eventhandlers_.end();
              iter++)
            {
               _names.push_back(iter->second->getName());
            }
      }

      void sessionList::checkLimitsAndAlerts()
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         std::map<t_long, eventHandler*>::iterator iter;
         for (iter = eventhandlers_.begin();
              iter != eventhandlers_.end();
              iter++)
            {
               // Check for any alerts.
               iter->second->handleAlerts();
               // Check if any torrents have seeded more/longer than
               // the limit; if so, stop them.
               iter->second->checkSeedLimits();
            }
      }

      void sessionList::updateElapsedOrSeedCounter()
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         std::map<t_long, eventHandler*>::iterator iter;
         for (iter = eventhandlers_.begin();
              iter != eventhandlers_.end();
              iter++)
            {
               iter->second->updateElapsedOrSeedCounter();
            }
      }

      void sessionList::erase(t_long const & _session)
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         std::map<t_long, eventHandler*>::iterator iter = eventhandlers_.find(_session);

         if (iter == eventhandlers_.end())
            {
               return;
            }

         eventHandler* eventhandler = iter->second;

         eventhandler->shutdown();

         delete eventhandler;
         eventhandler = 0;

         eventhandlers_.erase(iter);
      }

      void sessionList::deleteAll()
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         const t_uint numberOfSessions = size();

         std::map<t_long, eventHandler*>::iterator iter;
         for (iter = eventhandlers_.begin();
              iter != eventhandlers_.end();
              iter++)
            {
               eventHandler* eh = iter->second;

               BTG_MNOTICE(logWrapper(), "Deleting session no. " << eh->getSession());

               VERBOSE_LOG(logWrapper(), verboseFlag_, "Terminating session " << 
                           iter->first << "/" << numberOfSessions << ".");

               delete eh;
               eh = 0;
            }

         eventhandlers_.clear();
      }

      void sessionList::shutdown()
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         std::map<t_long, eventHandler*>::iterator iter;
         for (iter = eventhandlers_.begin();
              iter != eventhandlers_.end();
              iter++)
            {
               eventHandler* eh = iter->second;
               eh->shutdown();
            }
      }

#if BTG_OPTION_SAVESESSIONS
      void sessionList::serialize(btg::core::externalization::Externalization* _e, 
                                  bool const _dumpFastResume)
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         // Number of sessions
         t_int count = size();
         _e->intToBytes(&count);

         std::map<t_long, btg::daemon::eventHandler*>::iterator iter;
         for (iter = eventhandlers_.begin(); iter != eventhandlers_.end(); iter++)
            {
               eventHandler* eh = iter->second;
               BTG_MNOTICE(logWrapper(), "Serializing session " << iter->first);
               eh->serialize(_e, _dumpFastResume);
            }
      }
#endif

      t_uint sessionList::size() const
      {
         return eventhandlers_.size();
      }

      void sessionList::updateFilter(IpFilterIf* _filter)
      {
         boost::mutex::scoped_lock interface_lock(interfaceMutex_);

         for (std::map<t_long, btg::daemon::eventHandler*>::iterator iter = eventhandlers_.begin(); 
              iter != eventhandlers_.end(); 
              iter++)
            {
               iter->second->updateFilter(_filter);
            }
      }

      sessionList::~sessionList()
      {

      }

   } // namespace daemon
} // namespace btg
