using Gtk;
using Halberd;

public class MainWindow: Window
{
    public AccelGroup accel;

    private HeaderBar toolbar;

    private GLArea viewport;

    private Gtk.Button button_new;
    private Gtk.Button button_open;

    public MainWindow()
    {
        accel = new Gtk.AccelGroup();
        add_accel_group(accel);
        set_events(Gdk.EventMask.ALL_EVENTS_MASK);

        // Init. Structure
        toolbar = new Gtk.HeaderBar();
        toolbar.title = "Halberd RPG Engine";
        toolbar.show_close_button = true;
        toolbar.decoration_layout = "menu:close";

        // Init. Widgets
        viewport = new GLArea();
        viewport.realize.connect(prepare_viewport);
        viewport.has_depth_buffer = true;
        viewport.auto_render = true;
        viewport.set_required_version(3, 3);

        button_new = new Gtk.Button.from_icon_name("document-new-symbolic", IconSize.LARGE_TOOLBAR);
        button_open = new Gtk.Button.from_icon_name("document-open-symbolic", IconSize.LARGE_TOOLBAR);

        toolbar.pack_start(button_new);
        toolbar.pack_start(button_open);

        this.set_titlebar(toolbar);
        this.add(viewport);

        this.window_position = WindowPosition.CENTER;
        this.set_default_size(1024, 768);
        this.set_border_width(3);
        this.destroy.connect(on_exit);
        this.show_all();
    }

    private void on_exit()
    {
        Editor.cleanup_render();
        Gtk.main_quit();
    }

    private void prepare_viewport()
    {
        viewport.make_current();
        Editor.init_render();
    }
}
