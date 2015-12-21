using Gtk;

public class SidePane : Box
{
    private Gdk.Pixbuf loaded_tileset;

    private ComboBoxText                 tileset_selector;
    private Box                          control_box;
    private IconView                     tile_selector;
    private TreeStore                    selector_data;
    private Gee.ArrayList<ResourceEntry> tileset_list;

    private Button button_add;
    private Button button_rm;

    public SidePane()
    {
        this.orientation = Orientation.VERTICAL;
        this.spacing = 6;
        this.margin = 12;
        tileset_list = new Gee.ArrayList<ResourceEntry>();

        init_structure();
        init_content();
        connect_signals();
    }

    private void init_structure()
    {
        control_box = new Box(Orientation.HORIZONTAL, 6);

        this.pack_end(control_box, false, false);
    }

    private void init_content()
    {
        tileset_selector = new ComboBoxText();
        tile_selector = new IconView();
        button_add = new Button.from_icon_name("list-add-symbolic");
        button_rm = new Button.from_icon_name("list-remove-symbolic");

        tileset_selector.set_button_sensitivity(SensitivityType.AUTO);
        button_rm.set_sensitive(false);

        control_box.pack_start(button_add, false, false);
        control_box.pack_start(button_rm, false, false);
        this.pack_start(tileset_selector, false, false);
        this.pack_start(tile_selector, true, true);
    }

    private void connect_signals()
    {
        button_add.clicked.connect(() =>
        {
            Popover select_pop = new Popover(button_add);
            ResourceSelector select_widget = new ResourceSelector("Select a tileset", "png");
            select_pop.add(select_widget);
            select_pop.modal = true;
            select_widget.respond.connect((response) =>
            {
                select_pop.hide();
                tileset_list.add(select_widget.get_selected());
                tileset_selector.append_text(tileset_list[tileset_list.size - 1].name);
            });
            select_pop.show_all();
        });

        tileset_selector.changed.connect(build_tiles);
    }

    private void build_tiles()
    {
        int id = tileset_selector.get_active();
        if(id < 0)
            return;

        try {
            loaded_tileset = new Gdk.Pixbuf.from_file(tileset_list[id].construct_path());
        } catch(Error e) {
            app.display_warning("Can't load tileset: " + e.message);
        }
        // TODO: Iterate through tiles and break off, generate the data
    }
}
