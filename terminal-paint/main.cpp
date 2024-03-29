#include "App.h"
using namespace std;

map<string, vector<string>> create_brush_map();
void validate_brush_map(map<string, vector<string>> brush_map);

int main()
{
	const string version = "v0.2.0";
	map<string, vector<string>> brush_map = create_brush_map();
	validate_brush_map(brush_map);
	App app(version, brush_map);
	app.run();
	return 0;
}

map<string, vector<string>> create_brush_map()
{
	// Some symbols might not render correctly here but still can in terminals.
	return {
		{ "0", { "choose with your keyboard" }},
		{ "1", { "│─┼┴├┬┤┘└┌┐" }},
		{ "2", { "║═╬╩╠╦╣╝╚╔╗" }},

		// The characters below can be changed freely.
		{ "3", { "█", "▓", "▒", "▌", "▐", "▄", "▀", "■", "▬", "▁", "▂", "▃", "▔", "▖", "▗", "▘", "▝", "▙", "▛", "▜", "▟", "▚", "▞"  }},
		{ "4", { "🔳", "🔲", "⬜", "🟥", "🟧", "🟨", "🟩", "🟦", "🟪", "🟫", "✅", "☑", "❎", "❌", "✖", "×", "╳", "╱", "╲", "🛑", "☢", "☣", "⚠", "❗", "❕", "🚷", "🚫", "⛔", "🏴", "🏁", "🏳", "🚩" }},
		{ "5", { "…", "·", "•", "◘", "⭕", "○", "◙", "°", "º", "☼", "ø", "╭", "╮", "╯", "╰" }},
		{ "6", { "↑", "↓", "⬆", "⬇", "▲", "▼", "🔼", "🔽", "↕", "↨", "←", "→", "⬅", "➡", "↔", "◄", "►", "«", "»", "‹", "›", "◀", "▶", "⏪", "⏩", "⏮", "⏭", "⏯", "⏸", "⏹", "🔁" }},
		{ "7", { "¤", "$", "¢", "€", "£", "¥", "₧", "©", "®", "™" }},
		{ "8", { "±", "×", "≤", "≥", "≈", "≡", "√", "⌠", "⌡", "∟", "⌐", "¬" }},
		{ "9", { "☺", "☻", "♥", "♦", "♣", "♠", "🃏", "🎴", "♪", "♫", "⌂", "∩", "¦", "†", "‡" }},
		{ "a", { "α", "ß", "Γ", "π", "Σ", "σ", "µ", "τ", "Φ", "Θ", "Ω", "δ", "∞", "φ", "ε" }},
		{ "b", { "🔒", "🔑", "🗝", "🔧", "⛏", "🔨", "🪓", "🔪", "🗡", "✂", "🏹", "🔫", "🧨", "💣", "🛢", "💉", "🔮", "🧭", "💊", "🩹", "⛑", "🦺", "🧤", "🥾", "🛡", "🤿", "🥊", "🧲", "🔍", "💡", "🔦", "🕯", "🧰", "📱", "📞", "☎" }},
		{ "c", { "💾", "💿", "📀", "💽", "⚙", "🔍", "🔒", "🔑", "🔗", "🗑", "🔅", "🔆", "⏰", "🔈", "🔉", "🔊", "🎧", "🔔", "🛒", "💻", "🧪", "⚗", "🔭", "⚖", "📿", "🧿", "📣", "🥁", "🎺", "🎸", "📚", "📓", "📕", "📗", "📘", "📙" }},
		{ "d", { "🎒", "🛄", "🛅", "💼", "👜", "👛", "👝", "💰", "🏧", "💲", "💸", "💵", "💶", "💴", "💷", "💳", "🧧", "🛍", "🎁", "📦", "🥇", "🥈", "🥉", "🏅", "🎖", "🏆", "💎", "💍", "♦", "🎰", "🎲" }},
		{ "e", { "♥", "💘", "💖", "💗", "💓", "💙", "💚", "💛", "💜", "🧡", "💝", "💞", "💟", "🖤", "❤", "💌", "❣", "🤎", "🤍", "💔", "💕" }},
		{ "f", { "💀", "☠", "🦴", "👀", "👁", "👣", "🐾", "💤", "💢", "옷", "🤺", "⛄", "☃", "🤖", "🗿", "👤", "👹", "👿", "👻", "🐜", "⚰", "⚱", "🏺", "⛓", "🕸", "⚔", "⚓", "🧩", "✉", "📦", "📪", "📮", "🪑" }},
		{ "g", { "🎉", "🎊", "🎆", "🎇", "💥", "🔥", "🩸", "💧", "💦", "🌊", "🧊", "❄", "⚡", "🌩", "🌪", "💨" }},
		{ "h", { "💐", "🌸", "🏵", "🌹", "🌺", "🌻", "🌼", "🌷", "🥀", "☘", "🌱", "🌲", "🌳", "🌴", "🌵", "🌾", "🌿", "🍀", "🍁", "🍂", "🍃" }},
		{ "i", { "🌡", "⌛", "⏳", "⏰", "🕐", "🕑", "🕒", "🕓", "🕔", "🕕", "🕖", "🕗", "🕘", "🕙", "🕚", "🕛", "🕜", "🕝", "🕞", "🕟", "🕠", "🕡", "🕢", "🕣", "🕤", "🕥", "🕦", "🕧" }},
		{ "j", { "🌐", "🌍", "🌎", "🌏", "🪐", "☀", "🌞", "🌙", "🌑", "🌒", "🌓", "🌔", "🌕", "🌖", "🌗", "🌘", "🌚", "🌝", "🌛", "🌜", "🌠", "☄", "⭐", "🌟", "✨", "💫" }},
		{ "k", { "🐒", "🦍", "🦧", "🐕", "🐈", "🐅", "🐆", "🐎", "🦌", "🦏", "🐃", "🐄", "🐖", "🐏", "🐐", "🐪", "🦘", "🦨", "🐁", "🦔", "🐇", "🦆", "🐓", "🦎", "🐊", "🐢", "🐍", "🐉", "🦖", "🦀", "🐝", "🦂", "🕷", "🦇", "🦠" }},
		{ "l", { "🍽", "🍴", "🥄", "🥢", "🥝", "🥥", "🍇", "🍈", "🍉", "🍊", "🍋", "🍌", "🍍", "🥭", "🍎", "🍏", "🍐", "🍑", "🍒", "🍓", "🍅", "🍆", "🌽", "🌶", "🍄", "🥑", "🥒", "🥬", "🥦", "🥔", "🍠", "🧄", "🧅", "🥕", "🌰", "🥜" }},
		{ "m", { "🍞", "🥯", "🥨", "🥐", "🧇", "🥞", "🥗", "🥪", "🍜", "🥡", "🍟", "🥘", "🥣", "🥟", "🍚", "🥫" }},
		{ "n", { "🥧", "🍦", "🍧", "🍨", "🥮", "🥠", "🍩", "🍪", "🎂", "🍰", "🧁", "🍫", "🍬", "🍭", "🍡", "🍮", "🍯", "🧃", "☕", "🍵", "🧉", "🍶", "🍾", "🍷", "🍸", "🍹", "🍺", "🍻", "🥂", "🥃", "🥤" }},
	};
}

void validate_brush_map(map<string, vector<string>> brush_map)
{
	if (brush_map.size() > 36)
	{
		ynot::print("Error: the brush map must have <= 36 rows.");
		throw std::out_of_range("The brush map must have <= 36 rows.");
	}
	size_t i = 0;
	for (const auto& row : brush_map)
	{
		size_t n = row.second.size();
		if (n > 36)
		{
			ynot::print("Error: each row in the brush map must have <= 36 elements.");
			ynot::print("\nRow " + row.first + " has " + to_string(n) + " elements.");
			throw std::out_of_range("Each row in the brush map must have <= 36 elements.");
		}
	}
}
