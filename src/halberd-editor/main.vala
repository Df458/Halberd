HalberdEditor app;

public static int main (string[] args)
{
    Gtk.init(ref args);
    app = new HalberdEditor(args);
    return app.runall();
}
