<div align="center">

# 📁 Nova Explorer

### A Modern Windows File Manager built with C++17 & Qt 6

A lightweight, feature-rich desktop file manager designed with a clean interface, efficient navigation, and modern desktop application architecture.

![C++](https://img.shields.io/badge/C++-17-blue?style=for-the-badge&logo=cplusplus)
![Qt](https://img.shields.io/badge/Qt-6-41CD52?style=for-the-badge&logo=qt)
![Platform](https://img.shields.io/badge/Platform-Windows-blue?style=for-the-badge)
![License](https://img.shields.io/badge/License-MIT-yellow?style=for-the-badge)

</div>

---

# 📖 About

**Nova Explorer** is a modern desktop File Manager developed using **C++17** and the **Qt 6 Framework**. The project focuses on creating a fast, intuitive, and user-friendly file management experience while demonstrating modern desktop application development practices.

Built using Qt's **Model-View architecture**, the application allows users to efficiently browse, search, organize, and manage files and folders through a clean graphical interface. The project follows an object-oriented and modular design, making the codebase easy to understand, maintain, and extend.

Nova Explorer was developed as a portfolio project to showcase practical software engineering skills, GUI development with Qt, and real-world filesystem programming.

---

# ✨ Features

## 📂 File Navigation

- Tree View Navigation
- Detailed File Explorer
- Back / Forward Navigation
- Up Directory Navigation
- Refresh Current Directory
- Address Bar Navigation

---

## 🔍 Search

- Real-Time File & Folder Search
- Instant Filtering using `QSortFilterProxyModel`

---

## 📄 File Operations

- 📁 Create New Folder
- ✏️ Rename Files & Folders
- 📋 Copy
- ✂️ Cut
- 📥 Paste
- 🗑 Delete
- ✅ Multi-Selection Support

---

## 🖥 User Experience

- Modern Desktop Interface
- Native Windows File Icons
- Right-Click Context Menu
- Keyboard Shortcuts
- Status Bar Notifications
- Persistent Settings using **QSettings**

---

## 📑 File Properties

Displays detailed information including:

- Name
- File Type
- File Size
- File Location
- Creation Date
- Last Modified Date
- File Permissions

---

# 🛠 Built With

- **C++17**
- **Qt 6**
- **Qt Widgets**
- **QFileSystemModel**
- **QSortFilterProxyModel**
- **Qt Signals & Slots**
- **Object-Oriented Programming**
- **CMake**

---

# ⌨ Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| Ctrl + C | Copy |
| Ctrl + X | Cut |
| Ctrl + V | Paste |
| Ctrl + Shift + N | New Folder |
| F2 | Rename |
| Delete | Delete Selected |

---

# 📁 Project Structure


```
NovaExplorer/
│
├── main.cpp                         
│
├── mainwindow.h                    
├── mainwindow.cpp                   
├── mainwindow.ui                    
│
├── navigationmanager.h              
├── navigationmanager.cpp            
│
├── fileoperations.h                 
├── fileoperations.cpp               
│
├── searchmanager.h                  
├── searchmanager.cpp                
│
├── propertiesdialog.h            
├── propertiesdialog.cpp             
├── propertiesdialog.ui              
│
├── utility.h                       
├── utility.cpp                      
│
├── CMakeLists.txt                   
└── README.md                       
```

---

# 📸 Screenshots

| Home |
|------|
| <img width="1372" height="884" alt="image" src="https://github.com/user-attachments/assets/c0943e7f-d6a2-46c6-a157-38997fa7586b" />
 

| Properties |
|------------|
| <img width="580" height="652" alt="image" src="https://github.com/user-attachments/assets/7e4b4fb2-1af4-40ce-bc61-47556b322161" />
 

---

# 🚀 Future Roadmap

### Version 1.1

- Breadcrumb Navigation
- Drag & Drop Support
- Bookmarks
- Quick Access

### Version 1.2

- Dark Mode
- File Preview Pane
- Tabs
- Folder Size Calculation

### Version 2.0

- Batch Rename
- Advanced Search
- ZIP Compression
- Duplicate File Finder
- Plugin Support

---

# 🎯 Learning Outcomes

This project demonstrates practical knowledge of:

- Object-Oriented Programming
- Desktop GUI Development
- Qt Model-View Architecture
- Signals & Slots
- File System Programming
- Event-Driven Programming
- Modular Software Design
- Modern C++ Development

---

# 🤝 Contributing

Contributions are always welcome!

If you'd like to improve Nova Explorer:

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Open a Pull Request

---

# 👨‍💻 Developer

**Mannat**

B.Tech Computer Science & Engineering

---

<div align="center">

### ⭐ If you found this project helpful, consider giving it a star!

It helps support future development and motivates me to build more open-source projects.

</div>
