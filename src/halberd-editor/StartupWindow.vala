using Gtk;
using Halberd;

public class StartupWindow : Window
{
    Box main_box;
    Box empty_box;
    ListBox recent_box;
    ButtonBox action_box;

    CheckButton load_last_check;
    Button button_new;
    Button button_load;
    Button button_exit;

    Image empty_image;
    Label empty_label;
    Separator empty_separator;

    Gee.ArrayList<File> project_list;

    public StartupWindow(File directory, bool load_last_setting)
    {
        this.window_position = WindowPosition.CENTER;
        init_project_list(directory);
        init_structure();
        init_content(load_last_setting);
        connect_signals();
        this.show_all();
    }

    private void init_project_list(File dir)
    {
        project_list = new Gee.ArrayList<File>();
        try {
            var enumerator = dir.enumerate_children (FileAttribute.STANDARD_NAME, 0);

            FileInfo file_info;
            while ((file_info = enumerator.next_file ()) != null) {
                if(file_info.get_file_type() == FileType.DIRECTORY)
                    project_list.add(enumerator.get_child(file_info));
            }
        } catch(GLib.Error e) {
            app.display_warning("Error getting the project list: " + e.message + "\n");
            return;
        }
    }

    private void init_structure()
    {
        main_box = new Box(Orientation.VERTICAL, 6);
        recent_box = new ListBox();
        action_box = new ButtonBox(Orientation.HORIZONTAL);
        empty_box = new Box(Orientation.VERTICAL, 6);

        main_box.margin_top = 12;
        main_box.margin_bottom = 12;
        main_box.margin_start = 18;
        main_box.margin_end = 18;
        main_box.homogeneous = false;

        action_box.spacing = 6;
        action_box.layout_style = ButtonBoxStyle.START;

        recent_box.height_request = 256;
        recent_box.selection_mode = SelectionMode.NONE;

        if(project_list.size > 0)
            main_box.pack_start(recent_box, true, true);
        else
            main_box.pack_start(empty_box, true, true);
        main_box.pack_end(action_box, false, false);
        this.add(main_box);
    }

    private void init_content(bool load_last_setting)
    {
        load_last_check = new CheckButton.with_label("Load the last project on startup");
        button_new = new Button.with_label("Create New");
        button_load = new Button.with_label("Load Other\u2026");
        button_exit = new Button.with_label("Exit");
        empty_image = new Image.from_icon_name("folder-symbolic", IconSize.DIALOG);
        empty_label = new Label("Your projects folder is empty");
        empty_separator = new Separator(Orientation.HORIZONTAL);

        button_new.get_style_context().add_class(STYLE_CLASS_SUGGESTED_ACTION);
        empty_image.set_pixel_size(256);
        empty_image.set_opacity(0.25f);
        load_last_check.active = load_last_setting;

        empty_box.pack_start(empty_image, true, true);
        empty_box.pack_start(empty_label, false, false);
        empty_box.pack_end(empty_separator, false, false);
        main_box.pack_start(load_last_check, false, false);
        action_box.pack_start(button_new, true, false);
        action_box.pack_start(button_load, true, false);
        action_box.pack_start(button_exit, true, false);
        action_box.set_child_secondary(button_exit, true);

        foreach(File f in project_list) {
            Box fbox = new Box(Orientation.HORIZONTAL, 6);
            fbox.set_homogeneous(false);
            fbox.border_width = 6;
            Image icon = new Image.from_icon_name("folder", IconSize.DIALOG);
            Label title_label = new Label(f.get_basename());
            title_label.halign = Align.START;
            Label timestamp_label = new Label("Timestamp");
            Button open_button = new Button.with_label("Open");
            open_button.valign = Align.CENTER;
            open_button.clicked.connect(() => {
                app.load_project(f.get_path(), true);
            });

            fbox.pack_start(icon, false, false);
            fbox.pack_start(title_label, true, true);
            fbox.pack_end(open_button, false, false);
            fbox.pack_end(timestamp_label, false, false);
            recent_box.insert(fbox, -1);
        }
    }

    private void connect_signals()
    {
        this.destroy.connect(() => { app.window_destroyed(this); });

        button_new.clicked.connect(() => {
            app.new_project();
        });
        button_load.clicked.connect(() => {
            app.load_dialog();
        });
        button_exit.clicked.connect(() => { this.destroy(); });
        load_last_check.toggled.connect(() => { app.settings.set_boolean("open-last", load_last_check.active); });
    }
}
