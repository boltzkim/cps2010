using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ProcessTest2
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Hello World!");


            Process[] prcessss = new Process[5];


            for (int i = 0; i < prcessss.Length; i++)
            {
                prcessss[i] = new Process();

                prcessss[i].StartInfo.FileName = "D:\\01.dev\\2017\\01.lvc\\cps2010\\Release\\cps2010.exe";

                prcessss[i].StartInfo.CreateNoWindow = false;
                prcessss[i].StartInfo.UseShellExecute = false;
                //prcessss[i].StartInfo.RedirectStandardOutput = true;
                //prcessss[i].StartInfo.RedirectStandardError = true;
                prcessss[i].StartInfo.RedirectStandardInput = true;


                //prcessss[i].StartInfo.FileName = "cmd.exe";
                //prcessss[i].StartInfo.Arguments = "/K E:\\01.dev\\2017\\17.lvc_router\\cps2010\\Debug\\cps2010.exe";

            }



            ConsoleKeyInfo cki;

            while (true)
            {

                cki = Console.ReadKey(true);

                if (cki.Key == ConsoleKey.A)
                {
                    for (int i = 0; i < prcessss.Length; i++)
                    {
                        prcessss[i].Start();
                    }
                }
                else if (cki.Key == ConsoleKey.S)
                {
                    foreach (var process in Process.GetProcessesByName("cps2010"))
                    {
                        process.Kill();
                    }
                }
                else if (cki.Key == ConsoleKey.Q)
                {
                    break;
                }

            }
        }
    
    }
}
