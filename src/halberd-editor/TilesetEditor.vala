using Gtk;

public class TilesetEditor : AssetEditor, Box
{
    public bool save()
    {
        return false;
    }

    public bool load(ResourceEntry entry)
    {
        return false;
    }

    public bool create_new(ResourceEntry entry)
    {
        return false;
    }
}
