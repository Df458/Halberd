class HalberdEditor : Gtk.Application
{
    MainWindow window;

    public HalberdEditor(string[] args)
    {
        Halberd.Game.Settings.init();
        Halberd.Editor.Cursor.init();
        window = new MainWindow();
    }

    public int runall()
    {
        Gtk.main();
        Halberd.Game.Settings.cleanup();
        return 0;
    }
}
