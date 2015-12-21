using Gtk;

public class SidePane : Box
{
    private Gdk.Pixbuf loaded_tileset;

    ScrolledWindow tile_scroll;

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
        tile_scroll = new ScrolledWindow(null, null);

        this.pack_end(control_box, false, false);
    }

    private void init_content()
    {
        tileset_selector = new ComboBoxText();
        selector_data = new TreeStore(1, typeof(Gdk.Pixbuf));
        tile_selector = new IconView.with_model(selector_data);
        button_add = new Button.from_icon_name("list-add-symbolic");
        button_rm = new Button.from_icon_name("list-remove-symbolic");

        tileset_selector.set_button_sensitivity(SensitivityType.AUTO);
        button_rm.set_sensitive(false);

        for(int i = 0; i < 32; ++i) {
            for(int j = 0; j < 32; ++j) {
                TreeIter iter;
                selector_data.append(out iter, null);
                //selector_data.set(iter, 0, new Gdk.Pixbuf(Gdk.Colorspace.RGB, true, 8, 32, 32), -1);
            }
        }
        tile_selector.set_pixbuf_column(0);
        tile_selector.item_padding = 1;
        tile_selector.spacing = 0;
        tile_selector.set_column_spacing(0);
        tile_selector.set_row_spacing(0);
        tile_selector.set_selection_mode(SelectionMode.BROWSE);
        //CellRenderer rtext = tile_selector.get_cells().nth_data(0);
        //rtext.set_visible(false);
        //rtext.set_sensitive(false);
        CellRenderer rbuf = tile_selector.get_cells().nth_data(0);
        rbuf.set_fixed_size(32, 32);
        rbuf.set_visible(false);

        control_box.pack_start(button_add, false, false);
        control_box.pack_start(button_rm, false, false);
        tile_scroll.add(tile_selector);
        this.pack_start(tileset_selector, false, false);
        this.pack_start(tile_scroll, true, true);
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

        TreeIter iter;
        selector_data.get_iter_first(out iter);
        for(int i = 0; i < 32; ++i) {
            for(int j = 0; j < 32; ++j) {
                //Gdk.Pixbuf buf;
                //selector_data.get(iter, 0, out buf);
                //loaded_tileset.copy_area(j * 32, i * 32, 32, 32, buf, 0, 0);
                selector_data.set(iter, 0, new Gdk.Pixbuf.subpixbuf(loaded_tileset, j * 32, i * 32, 32, 32));
                selector_data.iter_next(ref iter);
            }
        }

        CellRenderer rbuf = tile_selector.get_cells().nth_data(0);
        rbuf.set_visible(true);
        tile_selector.unselect_all();
    }
}
