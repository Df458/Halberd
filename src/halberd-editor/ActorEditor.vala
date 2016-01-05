using Gtk;

public class ActorEditor : AssetEditor, SettingsGrid
{
    private Box        sprite_box;
    private Label      sprite_value_label;
    private Button     sprite_button;
    private SpinButton speed_input;

    private CheckButton visible_check;
    private CheckButton solid_check;
    private CheckButton lock_check;
    private CheckButton orient_check;
    private CheckButton ghost_check;
    private unowned Halberd.Actor? actor = null;

    public ActorEditor()
    {
        this.margin = 18;

        init_content();
        connect_signals();

        this.show_all();
    }
    
    private void init_content()
    {
        sprite_box         = new Box(Orientation.HORIZONTAL, 6);
        sprite_value_label = new Label("None");
        sprite_button      = new Button.with_label("Choose\u2026");
        speed_input        = new SpinButton.with_range(0, 100, 1);

        visible_check = new CheckButton.with_label("Visible");
        solid_check   = new CheckButton.with_label("Solid");
        lock_check    = new CheckButton.with_label("Lock to Grid");
        orient_check  = new CheckButton.with_label("Orientable");
        ghost_check   = new CheckButton.with_label("Ghost");

        Pango.FontDescription descript = new Pango.FontDescription();
        descript.set_style(Pango.Style.ITALIC);
        Pango.AttrList attributes = new Pango.AttrList();
        attributes.insert(new Pango.AttrFontDesc(descript));
        sprite_value_label.set_attributes(attributes);
        sprite_value_label.sensitive = false;

        speed_input.sensitive = false;

        sprite_box.add(sprite_button);
        sprite_box.add(sprite_value_label);
        add("Spriteset", sprite_box, 0);
        add("Default Speed", speed_input, 1);
        add("Flags", visible_check, 2);
        add(null, solid_check, 2);
        add(null, lock_check, 2);
        add(null, orient_check, 2);
        add(null, ghost_check, 2);
    }

    private void connect_signals()
    {
        sprite_button.clicked.connect(() =>
        {
            Popover select_pop = new Popover(sprite_button);
            ResourceSelector select_widget = new ResourceSelector("Sewlect a spriteset", "spr");
            select_pop.add(select_widget);
            select_pop.modal = true;
            select_widget.respond.connect((response) =>
            {
                select_pop.hide();
                ResourceEntry entry = select_widget.get_selected();
            });
            select_pop.show_all();
        });
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
    }
}
