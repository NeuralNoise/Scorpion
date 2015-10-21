
public class Log:
   public Log(): end

   int a(string tag, string message):
      return print(LogManager.ASSERT,tag,message);
   end

   function int e(string tag,string message):
      return print(LogManager.ERROR,tag,message);
   end

   function int w(string tag,string message):
      return print(LogManager.WARN,tag,message);
   end

   function int i(string tag,string message):
      return print(LogManager.INFO,tag,message);
   end

   function int d(string tag,string message):
      return print(LogManager.ERROR,tag,message);
   end

   function int v(string tag,string message):
      return print(LogManager.VERBOSE,tag,message);
   end

   @ Log the data
   function int println(string TAG, string message):

   end

   function int print(int priority, string tag, string message):
      if(LogManager.isLogEnabled()):
         if(LogManager.getPriority() <= priority):
            return println(tag,message);
         end
         else:
            return 0;
         end
      end
      else:
         return 0;
      end
   end

endclass
