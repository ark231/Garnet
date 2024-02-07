namespace WomuYuro {
struct IndentLevel {
    int level;
    operator int() { return level; }
};
IndentLevel operator""_ind(unsigned long long value);
}  // namespace WomuYuro
