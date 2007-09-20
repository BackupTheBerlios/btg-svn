#include <iomanip>
#include "ext_printer.h"

#include <bcore/command/command.h>

namespace btg
{
   namespace core
   {
      namespace externalization
      {

         using namespace btg::core;

         externalizationPrinter::externalizationPrinter()
         {

         }

         externalizationPrinter::~externalizationPrinter()
         {

         }

         bool externalizationPrinter::setBuffer(btg::core::dBuffer & _buffer)
         {
            return true;
         }

         t_int externalizationPrinter::getBufferSize()
         {
	   // Not used.
	   return 0;
         }

         void externalizationPrinter::getBuffer(btg::core::dBuffer & _buffer)
         {

         }

         void externalizationPrinter::reset()
         {
            // std::cout << "reset()" << std::endl;
            if (parameters.size() > 0)
               {
                  std::cout << command_name << std::endl;
                  for (t_uint i = 0; i < command_name.size(); i++)
                     {
                        std::cout << "-";
                     }

                  std::string dir;
                  if (direction == TO_SERVER)
                     {
                        dir = "To Server (method call)";
                     }
                  else
                     {
                        dir = "From Server (method response)";
                     }

                  std::stringstream tbl;

                  // Construct table header
                  std::cout << std::endl << std::endl;
                  for (t_uint i = 0; i < command_method_name.size(); i++)
                     {
                        tbl << "=";
                     }
                  tbl << " ========== ";

                  for (t_uint i = 0; i < dir.size(); i++)
                     {
                        tbl << "=";
                     }

                  std::cout << tbl.str() << std::endl;
                  std::cout << "Method name";

                  t_uint max_size = 0;
                  if ((command_method_name.size() - 11) <= 0)
                     {
                        max_size = 11;
                     }
                  else
                     {
                        max_size = command_method_name.size() - 11;
                     }

                  for (t_uint i = 0; i < max_size; i++)
                     {
                        std::cout << " ";
                     }

                  std::cout << " Command ID Direction" << std::endl;
                  std::cout << tbl.str() << std::endl;
                  std::cout << command_method_name << " " << std::setw(10) << command_id << " " << dir << std::endl;
                  std::cout << tbl.str() << std::endl << std::endl;


                  std::vector<std::pair<std::string, std::string> >::iterator iter;

                  std::cout << "Parameters:" << std::endl;
                  std::cout << "~~~~~~~~~~~" << std::endl;

                  t_uint number = 1;
                  for (iter = parameters.begin(); iter != parameters.end(); iter++)
                     {
                        std::pair<std::string, std::string> p = *iter;

                        std::cout << "(" << number << ") " << p.first << " (" << p.second << ")" << std::endl;
                        number++;
                     }

                  std::cout << std::endl;
                  parameters.clear();
               }
         }

         t_byte externalizationPrinter::determineCommandType(t_int & _command)
         {
            return 1;
         }

         void externalizationPrinter::setDirection(btg::core::DIRECTION _dir)
         {
            direction = _dir;
         }

         t_byte externalizationPrinter::getCommand(t_int & _command)
         {
            return 1;
         }

         t_byte externalizationPrinter::setCommand(t_int const _command)
         {
            command_name = Command::getName(_command);
            command_method_name = getCommandName(_command);
            setParamInfo("command ID", true);
            command_id = _command;
            addByte(_command);

            return 1;
         }

         void externalizationPrinter::setParamInfo(std::string const& _name, bool _required)
         {
            current_param_name = _name;
            if(_required)
               {
                  current_param_required = "required";
               }
            else
               {
                  current_param_required = "optional";
               }
         }

         bool externalizationPrinter::boolToBytes(bool const _source)
         {
            std::pair<std::string, std::string> pair(current_param_name, "bool (Boolean), "+current_param_required);
            parameters.push_back(pair);

            success();
            return true;
         }

         bool externalizationPrinter::bytesToBool(bool & _destination)
         {
            success();

            return true;
         }

         bool externalizationPrinter::addByte(t_byte const _byte)
         {
            std::pair<std::string, std::string> pair(current_param_name, "byte (Integer), "+current_param_required);
            parameters.push_back(pair);

            success();

            return true;
         }

         bool externalizationPrinter::getByte(t_byte & _byte)
         {
            success();

            return true;
         }

         bool externalizationPrinter::addBytes(t_byteCP _bytes, t_uint const _size)
         {
            std::pair<std::string, std::string> pair(current_param_name+ "(number of bytes)", "byte (Integer), "+current_param_required);
            parameters.push_back(pair);

            std::pair<std::string, std::string> pair2(current_param_name + "(data)", "multiple bytes (base64 string for XMLRPC), "+current_param_required);
            parameters.push_back(pair2);

            success();

            return true;
         }

         bool externalizationPrinter::getBytes(t_byteP* _bytes, t_uint &_size)
         {
            success();

            return true;
         }

         bool externalizationPrinter::intToBytes(const t_int* _source)
         {
            std::pair<std::string, std::string> pair(current_param_name, "integer (Integer), "+current_param_required);
            parameters.push_back(pair);
            success();

            return true;
         }

         bool externalizationPrinter::bytesToInt(t_int *_destination)
         {
            success();

            return true;
         }

         bool externalizationPrinter::uintToBytes(const t_uint* _source)
         {
            std::pair<std::string, std::string> pair(current_param_name, "unsigned integer (Integer), "+current_param_required);
            parameters.push_back(pair);
            success();

            return true;
         }

         bool externalizationPrinter::bytesToUint(t_uint *_destination)
         {
            success();

            return true;
         }

         bool externalizationPrinter::longToBytes(const t_long* _source)
         {
            std::pair<std::string, std::string> pair(current_param_name, "long (Integer), "+current_param_required);
            parameters.push_back(pair);
            success();

            return true;
         }

         bool externalizationPrinter::uLongToBytes(const t_ulong* _source)
         {
            std::pair<std::string, std::string> pair(current_param_name, "unsigned long (Integer), "+current_param_required);
            parameters.push_back(pair);
            success();

            return true;
         }

         bool externalizationPrinter::bytesToLong(t_long *_destination)
         {
            success();

            return true;
         }

         bool externalizationPrinter::bytesToULong(t_ulong *_destination)
         {
            success();

            return true;
         }

         bool externalizationPrinter::floatToBytes(const t_float* _source)
         {
            std::pair<std::string, std::string> pair(current_param_name, "float (Double), "+current_param_required);
            parameters.push_back(pair);
            success();

            return true;
         }

         bool externalizationPrinter::bytesToFloat(t_float *_destination)
         {
            success();

            return true;
         }

         bool externalizationPrinter::intListToBytes(const t_intList* _source)
         {
            std::pair<std::string, std::string> pair(current_param_name, "int list (List of Integers), "+current_param_required);
            parameters.push_back(pair);
            success();

            return true;
         }

         bool externalizationPrinter::bytesToIntList(t_intList *_destination)
         {
            success();

            return true;
         }

         bool externalizationPrinter::longListToBytes(const t_longList* _source)
         {
            std::pair<std::string, std::string> pair(current_param_name, "long list (List of Integers), "+current_param_required);
            parameters.push_back(pair);
            success();

            return true;
         }

         bool externalizationPrinter::bytesToLongList(t_longList* _destination)
         {
            success();

            return true;
         }

         bool externalizationPrinter::stringListToBytes(const t_strList* _source)
         {
            std::pair<std::string, std::string> pair(current_param_name, "string list (List of Strings), "+current_param_required);
            parameters.push_back(pair);
            success();

            return true;
         }

         bool externalizationPrinter::bytesToStringList(t_strList *_destination)
         {
            success();

            return true;
         }

         bool externalizationPrinter::stringToBytes(const std::string* _source)
         {
            std::pair<std::string, std::string> pair(current_param_name, "string (String), "+current_param_required);
            parameters.push_back(pair);
            success();

            return true;
         }

         bool externalizationPrinter::bytesToString(std::string *_destination)
         {
            success();

            return true;
         }

         std::string externalizationPrinter::dumpData() const
         {
            std::string o("output");

            return o;
         }

      } // namespace externalization
   } // namespace core
} // namespace btg
