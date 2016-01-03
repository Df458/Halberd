using Gtk;

public class ActorEditor : AssetEditor, Grid
{
    private Separator separator;

    // Labels
    private Label sprite_label;
    private Label speed_label;
    private Label script_label;
    private Label flag_label;

    // Inputs
    private Button     sprite_button;
    private SpinButton speed_input;

    private Box         flag_box;
    private CheckButton visible_check;
    private CheckButton solid_check;
    private CheckButton lock_check;
    private CheckButton orient_check;
    private CheckButton ghost_check;
    private unowned Halberd.Actor? actor = null;

    public ActorEditor()
    {
        this.row_spacing = 6;
        this.column_spacing = 12;
        this.margin = 18;

        init_structure();
        init_content();
        connect_signals();

        this.show_all();
    }

    private void init_structure()
    {
        flag_box = new Box(Orientation.VERTICAL, 0);
        separator = new Separator(Orientation.HORIZONTAL);

        this.attach(separator, 0, 1, 2);
        this.attach(flag_box, 1, 4);
    }
    
    private void init_content()
    {
        sprite_label = new Label("Spriteset");
        speed_label  = new Label("Default Speed");
        script_label = new Label("Scripts");
        flag_label   = new Label("Flags");

        sprite_button = new Button.with_label("Choose\u2026");
        speed_input   = new SpinButton.with_range(0, 100, 1);

        visible_check = new CheckButton.with_label("Visible");
        solid_check   = new CheckButton.with_label("Solid");
        lock_check    = new CheckButton.with_label("Lock to Grid");
        orient_check  = new CheckButton.with_label("Orientable");
        ghost_check   = new CheckButton.with_label("Ghost");

        sprite_label.halign = Align.END;
        speed_label.halign  = Align.END;
        script_label.halign = Align.END;
        script_label.valign = Align.START;
        flag_label.halign   = Align.END;
        flag_label.valign   = Align.START;

        this.attach(sprite_label, 0, 0);
        this.attach(speed_label,  0, 2);
        this.attach(script_label, 0, 3);
        this.attach(flag_label,   0, 4);
        this.attach(sprite_button, 1, 0);
        this.attach(speed_input,   1, 2);
        flag_box.add(visible_check);
        flag_box.add(solid_check);
        flag_box.add(lock_check);
        flag_box.add(orient_check);
        flag_box.add(ghost_check);
    }

    private void connect_signals()
    {
    }

    public bool create_new(ResourceEntry entry)
    {
        actor = Halberd.create_actor_for_resource(entry.path, entry.name);
        if(actor == null)
            return false;
        sync();
        save();
        return actor == null;
    }

    public bool save()
    {
        if(actor == null)
            return false;
        actor.data.speed = (uint16)speed_input.value;
        uint8 flags = 0;
        if(visible_check.active)
            flags += Halberd.ActorFlags.VISIBLE;
        if(solid_check.active)
            flags += Halberd.ActorFlags.SOLID;
        if(lock_check.active)
            flags += Halberd.ActorFlags.LOCK_TO_GRID;
        if(orient_check.active)
            flags += Halberd.ActorFlags.CAN_ORIENT;
        if(!ghost_check.active)
            flags += Halberd.ActorFlags.BLOCK_WITH_SOLID;
        actor.data.flags = flags;

        stderr.printf("Data: %u, %d\n", actor.data.flags, actor.data.flags & Halberd.ActorFlags.VISIBLE);
        Halberd.save_actor();
        return true;
    }

    public bool load(ResourceEntry entry)
    {
        actor = Halberd.load_actor_from_resource(entry.path, entry.name);
        if(actor == null)
            return false;
        Halberd.register_actor(actor, entry.path, entry.name);
        sync();
        return true;
    }

    public void sync()
    {
        speed_input.value = actor.data.speed;
        visible_check.active = (actor.data.flags & Halberd.ActorFlags.VISIBLE) != 0;
        solid_check.active   = (actor.data.flags & Halberd.ActorFlags.SOLID) != 0;
        lock_check.active    = (actor.data.flags & Halberd.ActorFlags.LOCK_TO_GRID) != 0;
        orient_check.active  = (actor.data.flags & Halberd.ActorFlags.CAN_ORIENT) != 0;
        ghost_check.active   = (actor.data.flags & Halberd.ActorFlags.BLOCK_WITH_SOLID) == 0;

        stderr.printf("Data: %u, %d\n", actor.data.flags, actor.data.flags & Halberd.ActorFlags.VISIBLE);
    }
}
