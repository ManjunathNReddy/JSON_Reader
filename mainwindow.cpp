#include "mainwindow.h"

#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setWindowTitle("JSON Reader");

    // All widgets are part of the central widget
    QWidget* central_widget = new QWidget;

    m_button_load = new QPushButton(tr("Load JSON"));
    connect(m_button_load, SIGNAL(released()), this, SLOT(onLoadClicked()));

    m_text_display = new QTextEdit();
    m_text_display->setReadOnly(true);

    m_checkbox_clear = new QCheckBox(tr("Clear text on opening file"));

    // Layout the widgets in vertical fashion
    QVBoxLayout* main_layout = new QVBoxLayout;
    main_layout->addWidget(m_button_load);
    main_layout->addWidget(m_text_display);
    main_layout->addWidget(m_checkbox_clear);

    central_widget->setLayout(main_layout);


    this->setCentralWidget(central_widget);
}

MainWindow::~MainWindow()
{
}

// Parse JSON and return a tuple of <number of elements, combined x, combined y>
// Return <-1,-1,-1> in case of invalid JSON
std::tuple<int, int, int> MainWindow::parseJSON(QString json_string)
{
    std::tuple<int, int, int> result = std::make_tuple(-1,-1,-1);
    int num_elements = 0, sum_x = 0, sum_y = 0;

    // Parse JSON
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(json_string.toUtf8(),&jsonError);
    if (jsonError.error == QJsonParseError::NoError)
    {
        QJsonObject json_obj = doc.object();
        // Sample schema: "NumberOfPositions":<n> -> "Position0" -> "x":<x>,"y":<y>
        QJsonValue num_positions = json_obj.value(QString("NumberOfPositions"));
        num_elements = num_positions.toInt();

        // Read json values as per "NumberOfPositions"
        for(int i=0; i<num_elements; ++i)
        {
            QString position_string = QString("Position%1").arg(i);
            QJsonValue position = json_obj.value(position_string);

            // Access subobject
            QJsonObject position_item = position.toObject();
            QJsonValue x_value = position_item["x"];
            QJsonValue y_value = position_item["y"];

            // Update the x and y sums
            sum_x += x_value.toInt();
            sum_y += y_value.toInt();
        }
        // Update result
        result = std::make_tuple(num_elements,sum_x,sum_y);
    }

    return result;
}

// Convert parseJSON tuple to message suitable for display
QString MainWindow::toMessage(std::tuple<int, int, int> parse_results)
{
    QString result = QString(tr("\nThis is not a valid JSON document!"));

    // Format the results in case they are present
    if(std::get<0>(parse_results) > -1)
    {
        int num_elements = std::get<0>(parse_results);
        int sum_x = std::get<1>(parse_results);
        int sum_y = std::get<2>(parse_results);

        result = QString(tr("\nNumber of Elements: %1\n"
                            "Combined value of X: %2\n"
                            "Combined value of Y: %3")).arg(num_elements).arg(sum_x).arg(sum_y);
    }
    return result;
}

void MainWindow::showText(QString text)
{
    // Create a seperator line
    m_text_display->insertPlainText("\n");
    QString seperator("*");
    m_text_display->insertPlainText(seperator.repeated(30));

    // If results must be cleared, do it now
    if(m_checkbox_clear->isChecked())
    {
        m_text_display->clear();
    }
    // Show text
    m_text_display->insertPlainText (text);
}

void MainWindow::onLoadClicked()
{
    // Browse for the file
    QString file_name = QFileDialog::getOpenFileName(this,
                                                     tr("Open JSON"), "", tr("JSON Files (*.json);;All Files (*.*)"));
    if(file_name.size())
    {
        // Open the file
        QFile file(file_name);
        file.open(QIODevice::ReadOnly | QIODevice::Text);

        // Load the entire file into a string
        QString json_file = file.readAll();

        // Close the file
        file.close();

        // Parse JSON and display result
        QString message = toMessage(parseJSON(json_file));
        showText(message);
    }
}

