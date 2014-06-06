using System.IO;
using System.Diagnostics;

class FlamingTorchBuild
{
    public static void Copy(string sourceDirName, string destDirName)
    {
        // Get the subdirectories for the specified directory.
        DirectoryInfo dir = new DirectoryInfo(sourceDirName);
        DirectoryInfo[] dirs = dir.GetDirectories();

        if (!dir.Exists)
        {
            throw new DirectoryNotFoundException(
                "Source directory does not exist or could not be found: "
                + sourceDirName);
        }

        // If the destination directory doesn't exist, create it. 
        if (!Directory.Exists(destDirName))
        {
            Directory.CreateDirectory(destDirName);
        }

        // Get the files in the directory and copy them to the new location.
        FileInfo[] files = dir.GetFiles();
        foreach (FileInfo file in files)
        {
            string temppath = Path.Combine(destDirName, file.Name);

            try
            {
                file.CopyTo(temppath, false);
            }
            catch(Exception e)
            {
                string ignored = e.Message;
            }
        }

        foreach (DirectoryInfo subdir in dirs)
        {
            string temppath = Path.Combine(destDirName, subdir.Name);
            Copy(subdir.FullName, temppath);
        }
    }

    public static void CompileMaps(string From, string To, string Exe, string WorkingDirectory)
    {
        string CurrentDirectory = Directory.GetCurrentDirectory();

        Directory.SetCurrentDirectory(WorkingDirectory);

        if (!Directory.Exists(To))
        {
            Directory.CreateDirectory(To);
        }

        try
        {
            string[] files = Directory.GetFiles(From, "*.tmx");
			
			Console.WriteLine("Files in " + From + ":" + files.Length);

            foreach(string FileName in files)
            {
                ProcessStartInfo startInfo = new ProcessStartInfo();
                startInfo.CreateNoWindow = false;
                startInfo.UseShellExecute = false;
                startInfo.FileName = CurrentDirectory + "/" + Exe;
                startInfo.WindowStyle = ProcessWindowStyle.Hidden;
                startInfo.Arguments = "-dir \"" + Directory.GetCurrentDirectory() + "/" + To + "\" \"" + Directory.GetCurrentDirectory() + "/" + From + "/" +
                    Path.GetFileName(FileName) + "\"";

                using (Process exeProcess = Process.Start(startInfo))
                {
                    exeProcess.WaitForExit();
                }
            }
        }
        catch(Exception e)
        {
            string temp = e.Message;
        }

        Directory.SetCurrentDirectory(CurrentDirectory);
    }

    public static void MakePackages(string From, string To, string Exe)
    {
        DirectoryInfo dir = new DirectoryInfo(From);
        DirectoryInfo[] dirs = dir.GetDirectories();

        string CurrentDirectory = Directory.GetCurrentDirectory();

        foreach(DirectoryInfo subdir in dirs)
        {
            DeleteDirectory(Directory.GetCurrentDirectory() + "/" + From + "/" + subdir.Name + "/Logs");
            DeleteDirectory(Directory.GetCurrentDirectory() + "/" + From + "/" + subdir.Name + "/Preprocess");

    		Console.WriteLine("Packing " + subdir.Name + " to " + To + "/" + subdir.Name + ".package");

            ProcessStartInfo startInfo = new ProcessStartInfo();
            startInfo.CreateNoWindow = false;
            startInfo.UseShellExecute = false;
            startInfo.FileName = CurrentDirectory + "/" + Exe;
            startInfo.WindowStyle = ProcessWindowStyle.Hidden;
            startInfo.Arguments = "-dir \"" + Directory.GetCurrentDirectory() + "/" + From + "/" + subdir.Name + "\" \"\" -out \"" +
                Directory.GetCurrentDirectory() + "/" + To + "\"" + subdir.Name + ".package\"";

            using (Process exeProcess = Process.Start(startInfo))
            {
                exeProcess.WaitForExit();
            }
        }
    }

    public static void DeleteDirectory(string name)
    {
        try
        {
            Directory.Delete(name, true);
        }
        catch(Exception e)
        {
            string msg = e.Message;
        }
    }
};

Env.InputPath = "./PackageContent";
Env.OutputPath = "./Content/";
Env.InputResolver = Resolvers.Flatten(CreateDirectoryCache(Env.InputPath));
Env.CreateOutputDirectory = true;

DirectoryInfo dir = new DirectoryInfo("./PackageContent/");
DirectoryInfo[] dirs = dir.GetDirectories();

FlamingTorchBuild.DeleteDirectory("./PackageContent/PackageData");

foreach(DirectoryInfo subdir in dirs)
{
    if(subdir.Name.Contains("PackageData"))
        continue;

    FlamingTorchBuild.Copy("./PackageContent/" + subdir.Name, "./PackageContent/PackageData/" + subdir.Name);
}

FlamingTorchBuild.MakePackages("./PackageContent/PackageData/", "./Content/", "Packer.exe");

FlamingTorchBuild.DeleteDirectory("./PackageContent/PackageData");
