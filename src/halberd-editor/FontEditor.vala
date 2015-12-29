using Gtk;

public class FontEditor : AssetEditor, Box
{
    public string get_requested_type()
    {
        return "fnt";
    }

    public bool save()
    {
        return false;
    }

    public bool load(ResourceEntry entry)
    {
        return false;
    }
}
