using Gtk;

public class TilesetEditor : AssetEditor, AssetViewer, Bin
{
    private Box box;

    public bool save()
    {
        return false;
    }

    public bool load(ResourceEntry entry)
    {
        return false;
    }

    public bool create(ResourceEntry entry)
    {
        return false;
    }
}
