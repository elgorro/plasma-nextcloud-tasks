# Nextcloud Tasks for KDE Plasma

A KDE Plasma 6 desktop widget to display and manage tasks from a Nextcloud instance.

## Features (Planned)

- View tasks from your Nextcloud server
- Mark tasks as complete
- Create and edit tasks
- Filter by task list / calendar
- Configurable server connection

## Installation

### From Package

Download the latest `.plasmoid` file from [Releases](https://github.com/elgorro/plasma-nextcloud-tasks/releases) and install it:

```bash
plasmapkg2 -i nextcloud-tasks.plasmoid
```

### Manual Installation

Clone the repository and symlink or copy the package to your local plasmoids directory:

```bash
git clone https://github.com/elgorro/plasma-nextcloud-tasks.git
cd plasma-nextcloud-tasks
ln -s "$(pwd)/package" ~/.local/share/plasma/plasmoids/com.github.elgorro.nextcloud-tasks
```

## Development

### Testing the Widget

Use `plasmoidviewer` to preview the widget without installing it:

```bash
plasmoidviewer -a package
```

### Building a Plasmoid Package

```bash
cd package
zip -r ../nextcloud-tasks.plasmoid *
```

## Roadmap

See [ROADMAP.md](ROADMAP.md) for the development plan.

## License

This project is licensed under the GPL-3.0 License - see the [LICENSE](LICENSE) file for details.
