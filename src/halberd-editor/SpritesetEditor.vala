using Gtk;

public class SpritesetEditor : AssetEditor, Box
{
    public string get_requested_type()
    {
        return "spr";
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
