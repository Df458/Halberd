using Gtk;

public class TilesetEditor : AssetEditor, Box
{
    public string get_requested_type()
    {
        return "til";
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
