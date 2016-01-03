using Gtk;

public class BlankEditor : Bin
{
    private Image placeholder_image;

    public BlankEditor()
    {
        this.margin = 18;
        placeholder_image = new Image.from_icon_name("text-x-generic-symbolic", IconSize.DIALOG);
        placeholder_image.set_pixel_size(256);
        placeholder_image.set_opacity(0.25f);

        this.add(placeholder_image);

        this.show_all();
    }
}
