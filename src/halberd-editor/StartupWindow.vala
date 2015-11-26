using Gtk;
using Halberd;

public class StartupWindow : Window
{
    Box main_box;
    ListBox recent_box;
    ButtonBox action_box;

    CheckButton load_last_check;
    Button button_new;
    Button button_load;
    Button button_exit;

    public StartupWindow()
    {
        init_structure();
        init_content();
        connect_signals();
        this.show_all();
    }

    public void init_structure()
    {
        main_box = new Box(Orientation.VERTICAL, 6);
        recent_box = new ListBox();
        action_box = new ButtonBox(Orientation.HORIZONTAL);

        main_box.margin_top = 12;
        main_box.margin_bottom = 12;
        main_box.margin_left = 18;
        main_box.margin_right = 18;
        main_box.homogeneous = false;

        action_box.spacing = 6;
        action_box.layout_style = ButtonBoxStyle.START;

        main_box.pack_start(recent_box, true, true);
        main_box.pack_end(action_box, false, false);
        this.add(main_box);
    }

    public void init_content()
    {
        load_last_check = new CheckButton.with_label("Load the last project on startup");
        button_new = new Button.with_label("Create New");
        button_load = new Button.with_label("Load Other\u2026");
        button_exit = new Button.with_label("Exit");

        button_new.get_style_context().add_class(STYLE_CLASS_SUGGESTED_ACTION);

        main_box.pack_start(load_last_check, false, false);
        action_box.pack_start(button_new, true, false);
        action_box.pack_start(button_load, true, false);
        action_box.pack_start(button_exit, true, false);
        action_box.set_child_secondary(button_exit, true);
    }

    public void connect_signals()
    {
        this.destroy.connect(Gtk.main_quit);

        // TODO: Make a way of getting the name and default in
        button_new.clicked.connect(() => {
            app.new_project();
        });
        button_exit.clicked.connect(() => { this.destroy(); });
        load_last_check.toggled.connect(() => { app.settings.set_boolean("open-last", load_last_check.active); });
    }
}
