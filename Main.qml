import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

ApplicationWindow {
    visible: true
    width: 1400
    height: 800
    title: "نظام إدارة المدرسة"
    Material.theme: Material.Dark
    Material.primary: Material.DeepPurple
    Material.accent: Material.Cyan

    // لوحة الألوان المتناسقة
    property color primaryColor: Qt.rgba(0,0,0,1)      // أزرق غامق
    property color secondaryColor: "#34495E"    // أزرق متوسط
    property color accentColor: "#3498DB"       // أزرق فاتح
    property color successColor: "#2ECC71"      // أخضر
    property color warningColor: "#F39C12"      // برتقالي
    property color dangerColor: "#E74C3C"       // أحمر
    property color textColor: "#ECF0F1"         // نص فاتح
    property color lightTextColor: "#BDC3C7"    // نص متوسط

    // نموذج بيانات محسن
    ListModel {
        id: personModel
        ListElement {
            person_id: "001"
            first_name: "أحمد"
            second_name: "محمد"
            third_name: "علي"
            date_of_birth: "1990-05-15"
            gender: "ذكر"
            role: "مدير"
            status: "نشط"
            department: "الإدارة العامة"
            salary: "15000"
            avatar: "👨‍💼"
        }
        ListElement {
            person_id: "002"
            first_name: "فاطمة"
            second_name: "إبراهيم"
            third_name: "خالد"
            date_of_birth: "1985-12-22"
            gender: "أنثى"
            role: "مشرف"
            status: "نشط"
            department: "الموارد البشرية"
            salary: "12000"
            avatar: "👩‍💼"
        }
        ListElement {
            person_id: "003"
            first_name: "يوسف"
            second_name: "سعيد"
            third_name: "عمر"
            date_of_birth: "1995-08-07"
            gender: "ذكر"
            role: "موظف"
            status: "إجازة"
            department: "تقنية المعلومات"
            salary: "8000"
            avatar: "👨‍💻"
        }
        ListElement {
            person_id: "004"
            first_name: "مريم"
            second_name: "عبدالله"
            third_name: "أحمد"
            date_of_birth: "1992-03-10"
            gender: "أنثى"
            role: "محاسب"
            status: "نشط"
            department: "المالية"
            salary: "10000"
            avatar: "👩‍💰"
        }
        ListElement {
            person_id: "005"
            first_name: "خالد"
            second_name: "فيصل"
            third_name: "سليم"
            date_of_birth: "1988-11-28"
            gender: "ذكر"
            role: "مهندس"
            status: "مريض"
            department: "الهندسة"
            salary: "13000"
            avatar: "👨‍🔧"
        }
    }

    // خلفية متدرجة
    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: primaryColor }
            GradientStop { position: 1.0; color: secondaryColor }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // شريط الأدوات العلوي المحسن
        Rectangle {
            Layout.fillWidth: true
            height: 80
            color: primaryColor

            RowLayout {
                anchors.fill: parent
                anchors.margins: 15
                spacing: 20

                // أيقونة وعنوان محسن
                Row {
                    spacing: 15

                    Rectangle {
                        width: 50
                        height: 50
                        radius: 25
                        color: accentColor

                        Label {
                            anchors.centerIn: parent
                            text: "👥"
                            font.pixelSize: 24
                            color: textColor
                        }
                    }

                    Column {
                        anchors.verticalCenter: parent.verticalCenter

                        Label {
                            text: "نظام إدارة المدرسة"
                            font.pixelSize: 24
                            font.bold: true
                            color: textColor
                        }

                        Label {
                            text: "الإصدار 1.0 Pro"
                            font.pixelSize: 12
                            color: lightTextColor
                        }
                    }
                }

                Item { Layout.fillWidth: true }

                // شريط البحث المحسن
                Rectangle {
                    Layout.preferredWidth: 300
                    height: 45
                    radius: 22
                    color: secondaryColor
                    border.color: accentColor
                    border.width: 1

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 10

                        Label {
                            text: "🔍"
                            font.pixelSize: 18
                            color: lightTextColor
                        }

                        TextField {
                            id: searchField
                            Layout.fillWidth: true
                            placeholderText: "ابحث عن موظف..."
                            font.pixelSize: 14
                            color: textColor
                            placeholderTextColor: lightTextColor
                            background: Item {}
                            selectByMouse: true
                        }

                        Button {
                            text: "×"
                            flat: true
                            width: 25
                            height: 25
                            visible: searchField.text.length > 0
                            onClicked: searchField.clear()
                            Material.foreground: lightTextColor
                        }
                    }
                }

                // أزرار الإجراءات المحسنة
                Row {
                    spacing: 10

                    Button {
                        text: "➕ إضافة جديد"
                        Material.background: successColor
                        Material.foreground: textColor
                        font.bold: true
                        height: 45
                        hoverEnabled: true
                    }

                    Button {
                        text: "📊 إحصائيات"
                        Material.background: warningColor
                        Material.foreground: textColor
                        font.bold: true
                        height: 45
                        hoverEnabled: true
                        onClicked: statsDialog.open()
                    }

                    Button {
                        text: "📤 تصدير"
                        Material.background: accentColor
                        Material.foreground: textColor
                        font.bold: true
                        height: 45
                        hoverEnabled: true
                    }
                }
            }
        }

        // شريط الفلاتر
        Rectangle {
            Layout.fillWidth: true
            height: 60
            color: secondaryColor
            border.color: "#ffffff20"
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 15

                Label {
                    text: "فلترة حسب:"
                    color: textColor
                    font.bold: true
                }

                ComboBox {
                    id: statusFilter
                    model: ["الكل", "نشط", "إجازة", "مريض"]
                    currentIndex: 0
                    Layout.preferredWidth: 120
                    Material.background: primaryColor
                    Material.foreground: textColor
                }

                ComboBox {
                    id: departmentFilter
                    model: ["كل الأقسام", "الإدارة العامة", "الموارد البشرية", "تقنية المعلومات", "المالية", "الهندسة"]
                    currentIndex: 0
                    Layout.preferredWidth: 150
                    Material.background: primaryColor
                    Material.foreground: textColor
                }

                ComboBox {
                    id: roleFilter
                    model: ["كل الأدوار", "مدير", "مشرف", "موظف", "محاسب", "مهندس"]
                    currentIndex: 0
                    Layout.preferredWidth: 120
                    Material.background: primaryColor
                    Material.foreground: textColor
                }

                Item { Layout.fillWidth: true }

                Label {
                    text: "عرض: " + listView.count + " موظف"
                    color: lightTextColor
                }
            }
        }

        // قائمة البيانات المحسنة
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "transparent"
            clip: true

            ListView {
                id: listView
                anchors.fill: parent
                anchors.margins: 10
                clip: true
                model: personModel
                spacing: 8

                // رؤوس الأعمدة
                header: Rectangle {
                    width: listView.width
                    height: 50
                    radius: 8
                    color: primaryColor
                    border.color: accentColor
                    border.width: 1

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 15
                        spacing: 10

                        Label { text: "الصورة"; Layout.preferredWidth: 80; font.bold: true; color: textColor; horizontalAlignment: Text.AlignHCenter }
                        Label { text: "المعرف"; Layout.preferredWidth: 70; font.bold: true; color: textColor; horizontalAlignment: Text.AlignHCenter }
                        Label { text: "الاسم الكامل"; Layout.preferredWidth: 200; font.bold: true; color: textColor; horizontalAlignment: Text.AlignHCenter }
                        Label { text: "تاريخ الميلاد"; Layout.preferredWidth: 120; font.bold: true; color: textColor; horizontalAlignment: Text.AlignHCenter }
                        Label { text: "القسم"; Layout.preferredWidth: 140; font.bold: true; color: textColor; horizontalAlignment: Text.AlignHCenter }
                        Label { text: "الراتب"; Layout.preferredWidth: 100; font.bold: true; color: textColor; horizontalAlignment: Text.AlignHCenter }
                        Label { text: "الحالة"; Layout.preferredWidth: 100; font.bold: true; color: textColor; horizontalAlignment: Text.AlignHCenter }
                        Label { text: "الإجراءات"; Layout.fillWidth: true; font.bold: true; color: textColor; horizontalAlignment: Text.AlignHCenter }
                    }
                }

                delegate: Rectangle {
                    width: listView.width
                    height: 80
                    radius: 10
                    color: secondaryColor
                    border.color: "#ffffff20"
                    border.width: 1

                    // تأثير الهوفر
                    property bool isHovered: false

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: parent.isHovered = true
                        onExited: parent.isHovered = false
                    }

                    // خلفية الهوفر
                    Rectangle {
                        anchors.fill: parent
                        radius: parent.radius
                        color: accentColor
                        opacity: parent.isHovered ? 0.2 : 0
                        Behavior on opacity { NumberAnimation { duration: 200 } }
                    }

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 15
                        spacing: 10

                        // صورة الملف الشخصي
                        Rectangle {
                            Layout.preferredWidth: 80
                            Layout.preferredHeight: 50
                            radius: 25
                            color: accentColor

                            Label {
                                anchors.centerIn: parent
                                text: avatar
                                font.pixelSize: 20
                                color: textColor
                            }
                        }

                        Label {
                            text: person_id
                            Layout.preferredWidth: 70
                            horizontalAlignment: Text.AlignHCenter
                            color: lightTextColor
                            font.bold: true
                        }

                        Label {
                            text: first_name + " " + second_name + " " + third_name
                            Layout.preferredWidth: 200
                            horizontalAlignment: Text.AlignHCenter
                            color: textColor
                            font.bold: true
                        }

                        Label {
                            text: date_of_birth
                            Layout.preferredWidth: 120
                            horizontalAlignment: Text.AlignHCenter
                            color: lightTextColor
                        }

                        Label {
                            text: department
                            Layout.preferredWidth: 140
                            horizontalAlignment: Text.AlignHCenter
                            color: accentColor
                            font.bold: true
                        }

                        Label {
                            text: salary + " ريال"
                            Layout.preferredWidth: 100
                            horizontalAlignment: Text.AlignHCenter
                            color: successColor
                            font.bold: true
                        }

                        // حالة محسنة مع أيقونات
                        Rectangle {
                            Layout.preferredWidth: 100
                            Layout.preferredHeight: 35
                            radius: 17
                            color: {
                                if (status === "نشط") return successColor;
                                else if (status === "إجازة") return warningColor;
                                else if (status === "مريض") return dangerColor;
                                else return lightTextColor;
                            }

                            Row {
                                anchors.centerIn: parent
                                spacing: 5

                                Label {
                                    text: {
                                        if (status === "نشط") return "✅";
                                        else if (status === "إجازة") return "🏖️";
                                        else if (status === "مريض") return "🤒";
                                        else return "❓";
                                    }
                                    font.pixelSize: 12
                                    color: textColor
                                }

                                Label {
                                    text: status
                                    color: textColor
                                    font.bold: true
                                    font.pixelSize: 12
                                }
                            }
                        }

                        // أزرار الإجراءات المحسنة
                        Row {
                            Layout.fillWidth: true
                            spacing: 8

                            Button {
                                text: "👁️"
                                Material.background: accentColor
                                Material.foreground: textColor
                                implicitWidth: 40
                                implicitHeight: 35
                                hoverEnabled: true
                                ToolTip.visible: hovered
                                ToolTip.text: "عرض التفاصيل"
                            }

                            Button {
                                text: "✏️"
                                Material.background: warningColor
                                Material.foreground: textColor
                                implicitWidth: 40
                                implicitHeight: 35
                                hoverEnabled: true
                                ToolTip.visible: hovered
                                ToolTip.text: "تعديل البيانات"
                            }

                            Button {
                                text: "📧"
                                Material.background: successColor
                                Material.foreground: textColor
                                implicitWidth: 40
                                implicitHeight: 35
                                hoverEnabled: true
                                ToolTip.visible: hovered
                                ToolTip.text: "إرسال رسالة"
                            }

                            Button {
                                text: "🗑️"
                                Material.background: dangerColor
                                Material.foreground: textColor
                                implicitWidth: 40
                                implicitHeight: 35
                                hoverEnabled: true
                                ToolTip.visible: hovered
                                ToolTip.text: "حذف الموظف"
                            }
                        }
                    }
                }

                ScrollBar.vertical: ScrollBar {
                    policy: ScrollBar.AlwaysOn
                    background: Rectangle {
                        color: primaryColor
                        radius: 5
                    }
                    contentItem: Rectangle {
                        color: accentColor
                        radius: 5
                    }
                }
            }
        }

        // التذييل المحسن
        Rectangle {
            Layout.fillWidth: true
            height: 50
            color: primaryColor

            RowLayout {
                anchors.fill: parent
                anchors.margins: 15

                Label {
                    text: "نظام إدارة المدرسة"
                    color: textColor
                    font.bold: true
                }

                Label {
                    text: "•"
                    color: lightTextColor
                }

                Label {
                    text: "الإصدار 1.0 Pro"
                    color: accentColor
                    font.bold: true
                }

                Item { Layout.fillWidth: true }

                Label {
                    text: "العراق"
                    color: lightTextColor
                    font.pixelSize: 12
                }
            }
        }
    }

    // نافذة الإحصائيات
    Dialog {
        id: statsDialog
        title: "📊 إحصائيات الموظفين"
        width: 500
        height: 400
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        Material.background: primaryColor

        Column {
            anchors.fill: parent
            spacing: 20

            Rectangle {
                width: parent.width
                height: 60
                radius: 8
                color: accentColor

                Row {
                    anchors.centerIn: parent
                    spacing: 15

                    Label {
                        text: "👥"
                        font.pixelSize: 24
                        color: textColor
                    }

                    Column {
                        Label {
                            text: "إجمالي الموظفين"
                            color: textColor
                            font.bold: true
                        }
                        Label {
                            text: personModel.count + " موظف"
                            color: textColor
                            font.pixelSize: 18
                            font.bold: true
                        }
                    }
                }
            }

            Grid {
                columns: 2
                spacing: 10
                width: parent.width

                Rectangle {
                    width: (parent.width - 10) / 2
                    height: 80
                    radius: 8
                    color: accentColor

                    Column {
                        anchors.centerIn: parent
                        Label {
                            text: "✅ الموظفين النشطين"
                            color: textColor
                            font.bold: true
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                        Label {
                            text: "3 موظفين"
                            color: textColor
                            font.pixelSize: 16
                            font.bold: true
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                    }
                }

                Rectangle {
                    width: (parent.width - 10) / 2
                    height: 80
                    radius: 8
                    color: warningColor

                    Column {
                        anchors.centerIn: parent
                        Label {
                            text: "🏖️ في إجازة"
                            color: textColor
                            font.bold: true
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                        Label {
                            text: "1 موظف"
                            color: textColor
                            font.pixelSize: 16
                            font.bold: true
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                    }
                }

                Rectangle {
                    width: (parent.width - 10) / 2
                    height: 80
                    radius: 8
                    color: dangerColor

                    Column {
                        anchors.centerIn: parent
                        Label {
                            text: "🤒 مرضى"
                            color: textColor
                            font.bold: true
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                        Label {
                            text: "1 موظف"
                            color: textColor
                            font.pixelSize: 16
                            font.bold: true
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                    }
                }

                Rectangle {
                    width: (parent.width - 10) / 2
                    height: 80
                    radius: 8
                    color: successColor

                    Column {
                        anchors.centerIn: parent
                        Label {
                            text: "💰 متوسط الراتب"
                            color: textColor
                            font.bold: true
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                        Label {
                            text: "11,600 ريال"
                            color: textColor
                            font.pixelSize: 16
                            font.bold: true
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                    }
                }
            }
        }
    }
}
