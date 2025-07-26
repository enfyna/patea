<p align="center">
  <img src="https://github.com/user-attachments/assets/4ed2d152-1784-4b20-bf15-75b947021441" width=100>
</p>
<h1 align="center">
  Patea
</h1>
<p align="center">
  <img src=https://img.shields.io/badge/C-CF4647?style=for-the-badge&logo=c&logoColor=white>
  <img src=https://img.shields.io/badge/gtk-579D7C?style=for-the-badge&logo=gtk&logoColor=white>
  <img src=https://img.shields.io/badge/Sqlite-007ACC?style=for-the-badge&logo=Sqlite&logoColor=white>
</p>
<br><br>
<p align="center">
  Pardus Teacher
</p>
<br>

<p align="center">
  <img width=80% src="https://github.com/user-attachments/assets/09acb1d4-920b-45bf-a1a1-a86880a0cf05" />
  <img width=40% src="https://github.com/user-attachments/assets/6316d577-043b-47eb-8ae2-d15c961b3841" />
  <img width=40% src="https://github.com/user-attachments/assets/296188e4-66cd-4f5b-83e6-d81af103eb8e" />
  <img width=40% src="https://github.com/user-attachments/assets/9e12843f-1007-4254-9bea-0a816993b296" />
  <img width=40% src="https://github.com/user-attachments/assets/c20e19c7-aacc-4f75-af0f-7fb2240108cc" />
</p>

# Building

1. Clone the repo.

        git clone https://github.com/enfyna/patea
        cd patea

2. Install dependencies.

        sudo nala install make pkg-config sqlite3 libsqlite3-dev libvte-2.91-dev

3. Build the project.

        make -j

4. Run the project.

         ./build/patea

# Documentation

For documentation check the [wiki](https://github.com/enfyna/patea/wiki).

# Develop

### Clangd

generate compile_commands.json:

          bear -- make



