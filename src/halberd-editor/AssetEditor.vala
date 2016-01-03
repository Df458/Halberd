public interface AssetEditor : Gtk.Widget
{
    public abstract bool save();
    public abstract bool load(ResourceEntry entry);
    public abstract bool create_new(ResourceEntry entry);
}
