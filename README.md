# GameGrabber360

A high-performance, native C utility made to automate bulk file transfers for Xbox 360 and Original Xbox game repositories. 

## The Problem
For my xbox360 modding business i needed to manage bulk game installations across multiple storage drives wich typically required manually searching terabytes of unorganized directories to match client requests. This process was a massive time sink and was a perfect canidate for optimization.

## The Solution
GameGrabber360 eliminates the manual searching. It reads a batch list of requested game titles, scans the target drive, and utilizes a custom **Levenshtein Distance algorithm** to determine the closest folder match (accounting for typos or naming variations). 

Once a match has been found, it establishes a Inter-Process Communication (IPC) pipe to Windows `robocopy`, bypassing standard system-blocking calls to parse stdout and render a inline progress tracker in the terminal.

Through a config file the program can be configured to unique repositories. other prefrences can be set such as always downloading dlc files (when available) to save on manual transfers

### Technical Highlights
* **Pure Native C:** No third-party frameworks or heavy dependencies used. 
* **Memory Safe:** Uses bounded string manipulation (`snprintf`, `strncpy`) to prevent buffer overflows during path concatenation.
* **IPC Data Parsing:** Replaces standard `system()` calls with `_popen` to maintain thread execution and parse external application memory streams in real-time.
* **Windows API Integration:** Hooks directly into COM (`ole32`) for native GUI folder selection dialogs.
* **Config file** Native config file parser/creator for orignal xbox, 360, and games list file paths

## Usage
*Note: For now this tool requires a Windows environment (Windows 10/11) due to COM and Robocopy dependencies.*

1. Download the latest compiled executable from the **Releases** tab.
2. Ensure you have a target text file (e.g., `games.txt`) with one requested title per line.
3. Run `GameGrabber360.exe`. No installation is required.

## Future updates
1. Linux compatability
2. implement with web based client terminal
3. Free space validation before transfer