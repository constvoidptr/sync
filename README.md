# Sync

Sync is a [EuroScope](https://www.euroscope.hu/wp/) plugin that allows easy synchronization of the departure list between controllers.

By default, EuroScope only synchronizes changes to controllers that are online at the given moment. For everyone connecting later,
all clearance received flags and ground statuses have to be manually set again to achieve synchronization.

Sync addresses this nuisance by keeping track of all default EuroScope and [Ground Radar Plugin](https://vatsim-scandinavia.org/forums/topic/3462-ground-radar-plugin-15/)
statuses and introduces a new command called `.sync` that forces automatic re-synchronization of the departure list.

## Usage
`.sync <Airport ICAO>` - Synchronizes the departure list for the specified airport.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.