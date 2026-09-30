# lcd-smartie-scrollifneeded

**Description**:
- Scrolls through a text string within a designated area, given that the string's character count exceeds the area's. Otherwise, string is displayed statically.

**Format**:
- $dll(ScrollIfNeeded,1,[length]/[text string],[speed]/[empty frames])

**Clarification**:
- [length] = number of designated characters
- [speed] = how many characters scrolled per frame
- [empty frames] = once string is fully out of view, how many frames waited until new scroll cycle begins
