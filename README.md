# omp-mod (Custom Native Vehicle Extended open.mp Server)

Custom modified fork of the **open.mp** C++ server binary, featuring native support for custom vehicles (IDs 31000–31999), MSVC 2022 (v143) toolset compilation, and array bounds protection.

---

## 🌟 Key Features & Modifications

### 1. 🏎️ Native Custom Vehicle Support (IDs `31000..31999`)
- **Direct RPC 164 (`WorldVehicleAdd`) Support**: Server natively broadcasts `CreateVehicle(31190)` with model IDs in the `31000..31999` range.
- **No Client-Side Hot-Swapping Needed**: Vehicles stream directly with their custom DFF/TXD model indices on Android & PC clients without triggering client-side vehicle destruction/recreation (`cef:customvehicle:apply`).
- **Automatic Physics & Base Model Mapping**: Automatically maps custom vehicle IDs to base vehicle handling IDs, passenger seat counts, and audio settings (fallback to base model `411` or user-defined `base_vehicle_id`).

### 2. 🛡️ SDK & Component Array Bounds Protection
- **Extended Model Range Validation**: Updated `isValidVehicleModel`, `getVehiclePassengerSeats`, `getVehicleModelInfo`, and `getRandomVehicleColour` across `SDK/include/Server/Components/Vehicles/` to safely validate custom model range `30000..44999`.
- **Prevents Out-Of-Bounds Errors**: Prevents `Array index out of bounds` errors when querying model properties for IDs > 611.

### 3. 🛠️ MSVC 2022 (v143) Toolset Compatibility
- Removed strict Clang-only compiler flags.
- Fixed MSVC compiler compatibility issues (`__attribute__` macro support and non-constexpr `getBuffer` in `LegacyNetwork`).

---

## 🛠️ Requirements & Tools

* **CMake 3.22+**
* **Python 3.12+** with **Conan 2.x** (`pip install conan`)
* **Visual Studio 2022** (Desktop development with C++ / MSVC `v143` 32-bit `Win32`)

---

## 🚀 Building on Windows (Visual Studio 2022 MSVC)

1. **Clone the Repository**:
   ```bash
   git clone --recursive https://github.com/hieeseooes/omp-mod.git
   cd omp-mod
   ```

2. **Install Conan Package Manager**:
   ```powershell
   pip install conan
   ```

3. **Configure & Build Server Binary**:
   ```powershell
   mkdir build
   cd build
   cmake .. -A Win32 -T v143
   cmake --build . --config RelWithDebInfo
   ```

---

## 📦 Output Binaries

After compilation completes successfully, generated files will be available at:
- **Server Binary**: `build/Output/RelWithDebInfo/Server/omp-server.exe`
- **Server Components/Plugins**: `build/Output/RelWithDebInfo/Server/components/*.dll`

---

## 📁 Repository Structure

| Path | Content |
| ---- | ------- |
| `SDK/include/Server/Components/Vehicles/` | Patched Vehicle SDK headers supporting custom vehicle IDs |
| `Server/Source/` | Core open.mp server implementation |
| `Server/Components/Vehicles/` | Vehicle component implementation with custom vehicle bounds checks |
| `Server/Components/Pawn/Scripting/Vehicle/` | Pawn native bindings (`CreateVehicle`, `GetVehicleModelInfo`) |
