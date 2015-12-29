using Gtk;

public class ActorEditor : AssetEditor, Box
{
    public string get_requested_type()
    {
        return "act";
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
