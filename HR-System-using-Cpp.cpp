#include <wx/wx.h>
#include <iomanip>
#include <vector>
#include <sstream>

// Class for working hour calculations
// This class is responsible for calculating early arrival, late arrival, total working hours, and overtime for a specific day.
class MyClass {
public:
    // Constructor to initialize entry and exit times for a day
    MyClass(const std::string& Entry, const std::string& Exit)
        : Entry(Entry), Exit(Exit), early(0), late(0), hour(0), min(0), overtime(0) {
    }

    // Helper function to extract and convert a specific substring to an integer (used for hours and minutes)
    int extractAndConvert(const std::string& input, int startIndex, int length) {
        std::string subStr = input.substr(startIndex, length);
        return std::stoi(subStr);
    }

    // Calculate entry and exit hours and minutes from the provided strings
    void calculateWorkingHour() {
        hourIn = extractAndConvert(Entry, 0, 2);
        minIn = extractAndConvert(Entry, 3, 2);

        hourOut = extractAndConvert(Exit, 0, 2);
        minOut = extractAndConvert(Exit, 3, 2);
    }
    
    // Calculate early and late minutes compared to an official start time of 8:00 AM
    void calculateEarlyAndLateHours() {
        if (hourIn < 8 || (hourIn == 8 && minIn == 0)) {
            early = (8 * 60) - (hourIn * 60 + minIn);
        }
        else {
            late = (hourIn * 60 + minIn) - (8 * 60);
        }
    }

    // Calculate total working hours and overtime compared to an official end time of 5:00 PM
    void calculateWorking() {
        int totalInMinutes = (hourOut * 60 + minOut) - (hourIn * 60 + minIn);
        if (totalInMinutes < 0) {
            totalInMinutes += 24 * 60; // Adjust for overnight shifts
        }

        hour = totalInMinutes / 60;
        min = totalInMinutes % 60;

        int officialEndTime = 17 * 60;
        int actualEndTime = hourOut * 60 + minOut;
        if (actualEndTime > officialEndTime) {
            overtime = actualEndTime - officialEndTime;
        }
    }

    // Generate a daily summary of the calculated hours in a formatted string
    std::string displayDailyResults() {
        std::ostringstream oss;
        oss << "Early: " << early / 60 << ":" << std::setw(2) << std::setfill('0') << early % 60 << "\n";
        oss << "Late: " << late / 60 << ":" << std::setw(2) << std::setfill('0') << late % 60 << "\n";
        oss << "Total Working Hours: " << hour << ":" << std::setw(2) << std::setfill('0') << min << "\n";
        oss << "Overtime: " << overtime / 60 << ":" << std::setw(2) << std::setfill('0') << overtime % 60 << "\n";
        return oss.str();
    }

    // Getters for various calculated metrics
    int getTotalWorkMinutes() const { return hour * 60 + min; }
    int getEarlyMinutes() const { return early; }
    int getLateMinutes() const { return late; }
    int getOvertimeMinutes() const { return overtime; }

private:
    std::string Entry;  // Entry time in HH:MM format
    std::string Exit;   // Exit time in HH:MM format
    int hourIn, hourOut, minIn, minOut;  // Entry and exit times split into hours and minutes
    int early, late, hour, min, overtime; // Calculated metrics
};

// Define the application
class MyApp : public wxApp {
public:
    virtual bool OnInit(); // Initialize the application
};

// Define the main frame
class MyFrame : public wxFrame {
public:
    MyFrame();

private:
    wxTextCtrl* entryFields[6];  // Entry fields for each day
    wxTextCtrl* exitFields[6];   // Exit fields for each day
    wxTextCtrl* resultArea;      // Text area to display results

    void OnCalculate(wxCommandEvent& event); // Event handler for the "Calculate" button

    wxDECLARE_EVENT_TABLE(); // Event table declaration
};

// Event table for handling button clicks
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_BUTTON(1001, MyFrame::OnCalculate) // Bind button ID 1001 to OnCalculate method
wxEND_EVENT_TABLE()

// Application entry point
wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
    MyFrame* frame = new MyFrame();
    frame->Show(true); // Show the main frame
    return true;
}

// Constructor for the main frame
MyFrame::MyFrame()
    : wxFrame(NULL, wxID_ANY, "Work Hours Calculator", wxDefaultPosition, wxSize(500, 700)) {
    // Set the application icon
    SetIcon(wxIcon("icon.ico", wxBITMAP_TYPE_ICO));

    // Layout for the main frame
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText* instructions = new wxStaticText(this, wxID_ANY, "Enter Entry and Exit times (HH:MM):");
    mainSizer->Add(instructions, 0, wxALL | wxCENTER, 10);

    // Create fields for 6 days
    for (int i = 0; i < 6; ++i) {
        wxBoxSizer* daySizer = new wxBoxSizer(wxHORIZONTAL);

        wxStaticText* dayLabel = new wxStaticText(this, wxID_ANY, wxString::Format("Day %d:", i + 1));
        daySizer->Add(dayLabel, 0, wxALL, 5);

        entryFields[i] = new wxTextCtrl(this, wxID_ANY, "");
        daySizer->Add(entryFields[i], 1, wxALL, 5);

        exitFields[i] = new wxTextCtrl(this, wxID_ANY, "");
        daySizer->Add(exitFields[i], 1, wxALL, 5);

        mainSizer->Add(daySizer, 0, wxEXPAND);
    }

    // Button for calculating results
    wxButton* calculateButton = new wxButton(this, 1001, "Calculate");
    mainSizer->Add(calculateButton, 0, wxALL | wxCENTER, 10);

    // Text area for displaying results
    resultArea = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
    mainSizer->Add(resultArea, 1, wxALL | wxEXPAND, 10);

    SetSizer(mainSizer);
}

// Event handler for calculating weekly work hours
void MyFrame::OnCalculate(wxCommandEvent& event) {
    std::vector<std::string> entries(6), exits(6);
    for (int i = 0; i < 6; ++i) {
        entries[i] = entryFields[i]->GetValue().ToStdString();
        exits[i] = exitFields[i]->GetValue().ToStdString();
    }
    
    int totalWorkMinutes = 0, totalEarlyMinutes = 0, totalLateMinutes = 0, totalOvertimeMinutes = 0;

    std::ostringstream results;

    // Process each day's entry and exit times
    for (int i = 0; i < 6; ++i) {
        MyClass d(entries[i], exits[i]);
        d.calculateWorkingHour();
        d.calculateEarlyAndLateHours();
        d.calculateWorking();
        results << "Results for Day " << i + 1 << ":\n" << d.displayDailyResults() << "\n";

        totalWorkMinutes += d.getTotalWorkMinutes();
        totalEarlyMinutes += d.getEarlyMinutes();
        totalLateMinutes += d.getLateMinutes();
        totalOvertimeMinutes += d.getOvertimeMinutes();
    }

    // Weekly summary
    results << "\n=== Summary for the Week ===\n";
    results << "Total Working Hours: " << totalWorkMinutes / 60 << ":"
        << std::setw(2) << std::setfill('0') << totalWorkMinutes % 60 << "\n";
    results << "Total Early Hours: " << totalEarlyMinutes / 60 << ":"
        << std::setw(2) << std::setfill('0') << totalEarlyMinutes % 60 << "\n";
    results << "Total Late Hours: " << totalLateMinutes / 60 << ":"
        << std::setw(2) << std::setfill('0') << totalLateMinutes % 60 << "\n";
    results << "Total Overtime: " << totalOvertimeMinutes / 60 << ":"
        << std::setw(2) << std::setfill('0') << totalOvertimeMinutes % 60 << "\n";

    resultArea->SetValue(results.str()); // Display results in the text area
}
