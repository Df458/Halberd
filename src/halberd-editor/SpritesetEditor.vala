using Gtk;

public class SpriteEditor : AssetEditor, AssetViewer, Bin
{
    private SettingsGrid grid;
    private Box          anim_box;
    private ComboBoxText anim_combo;
    private Button       anim_add_button;
    private SpinButton   length_entry;
    private SpinButton   speed_entry;
    private ComboBoxText orient_combo;
    private unowned Halberd.Sprite? sprite = null;

    public SpriteEditor()
    {
        this.margin = 18;
        init_content();

        this.show_all();
    }

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
        //sprite = Halberd.create_sprite_for_resource(entry.path, entry.name);
        if(sprite == null)
            return false;
        //sync();
        save();
        return sprite == null;
    }

    private void init_content()
    {
        grid            = new SettingsGrid();
        anim_box        = new Box(Orientation.HORIZONTAL, 0);
        anim_combo      = new ComboBoxText();
        anim_add_button = new Button.from_icon_name("list-add-symbolic", IconSize.BUTTON);

        length_entry = new SpinButton.with_range(1, 1000, 1);
        speed_entry  = new SpinButton.with_range(0, 1000, 0.1);
        orient_combo = new ComboBoxText();

        anim_box.get_style_context().add_class(STYLE_CLASS_LINKED);
        anim_box.pack_start(anim_combo, true, true);
        anim_box.add(anim_add_button);

        grid.add("Animation", anim_box, 0);
        grid.add("Length", length_entry, 1);
        grid.add("Speed", speed_entry, 1);
        grid.add("Orientations", orient_combo, 1);
        //add("Offset", , 1);
        this.add(grid);
    }
}
