public interface AssetEditor : Gtk.Widget
{
    public abstract string get_requested_type();
    public abstract bool save();
    public abstract bool load(ResourceEntry entry);
}
