using Halberd.IO;

public class ResourceEntry : Object
{
    public string? path;
    public string name;
    public ResourceEntry(File file)
    {
        name = file.get_basename();
        path = app.path_to_content_path(file.get_parent().get_path());
    }

    public ResourceEntry.from_entry(string? inpath, string inname)
    {
        path = inpath;
        name = inname;
    }

    public string construct_path()
    {
        return make_path(path, name);
    }

    public File construct_file()
    {
        return file_from_resource(path, name);
    }
}
