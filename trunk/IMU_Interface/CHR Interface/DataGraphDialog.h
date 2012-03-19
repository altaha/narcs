#pragma once

// <adeel>
#include "SharedMemWrapper.h"
#include "SyncObjsWrapper.h"
// <adeel>

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::IO;

using namespace System::Runtime::InteropServices;

using namespace ZedGraph;
using namespace StopWatch;

#define	MAX_GRAPH_LISTS		255
#define	COLOR_COUNT			10
// <adeel>
#define PROCESSED_X_GYRO_NOISE_RANGE_HIGH_THRESHOLD  45.0
#define PROCESSED_X_GYRO_NOISE_RANGE_LOW_THRESHOLD   25.0
#define PROCESSED_Y_GYRO_NOISE_RANGE_HIGH_THRESHOLD  5.0
#define PROCESSED_Y_GYRO_NOISE_RANGE_LOW_THRESHOLD  -15.0

#define PROCESSED_X_ACCEL_NOISE_RANGE_HIGH_THRESHOLD  0.15
#define PROCESSED_X_ACCEL_NOISE_RANGE_LOW_THRESHOLD   -0.15
#define PROCESSED_Y_ACCEL_NOISE_RANGE_HIGH_THRESHOLD  0.15
#define PROCESSED_Y_ACCEL_NOISE_RANGE_LOW_THRESHOLD   -0.15
// </adeel>

struct IMU_Raw_Data{
		double time_s;
		double x_gyro;
		double y_gyro;
		double z_gyro;
		double x_accel;
		double y_accel;
		double z_accel;
};

namespace CHRInterface {

	/// <summary>
	/// Summary for DataGraphDialog
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class DataGraphDialog : public System::Windows::Forms::Form
	{
	public:
		DataGraphDialog(void)
		{
			// <adeel>
			this->IMUSharedMemory = gcnew SharedMemWrapper (sizeof(IMU_Raw_Data)*2, L"IMUSharedMemory", true);
			this->IMUSharedMemory->Start(0);
	
			this->IMUSharedMemoryMutex = gcnew MutexObjWrapper;
			this->IMUSharedMemoryMutex->initNamedMutex(L"IMUSharedMemoryMutex", true);
			// </adeel>
			// <Ahmed>
			this->IMUSharedMemoryEvent = gcnew EventObjWrapper;
			this->IMUSharedMemoryEvent->initNamedEvent(L"IMUSharedMemoryEvent", true, false);
			writeOneShot = false;
			// </Ahmed>

			InitializeComponent();

			graphTime = gcnew Stopwatch;
			
			dataGraphList = gcnew cli::array<RollingPointPairList^>(MAX_GRAPH_LISTS);
			this->dataListCount = 0;

			dataSendingEnabled = false;

			dataItemIndexes = gcnew cli::array<UInt32,1>(MAX_GRAPH_LISTS);
			
			autoSetAxes = true;
			this->dataHistorySize = 100;
			title = L"";
			xLabel = L"Time (s)";
			yLabel = L"Sensor Output";
			yMin = 0;
			yMax = 0;
			
			//timer1->Interval = 50;
			timer1->Interval = 20;
			time = 0;

			colors = gcnew cli::array<Color>(COLOR_COUNT);
			colors[0] = Color::Blue;
			colors[1] = Color::Red;
			colors[2] = Color::Green;
			colors[3] = Color::Brown;
			colors[4] = Color::DarkBlue;
			colors[5] = Color::DarkRed;
			colors[6] = Color::DarkGreen;
			colors[7] = Color::Cyan;
			colors[8] = Color::DarkKhaki;
			colors[9] = Color::Aquamarine;

			PopulateGraphSettings();
		}

	public:
		void SetFirwarmwareReference( FirmwareDefinition^ firmware )
		{
			this->firmware = firmware;

			// Clear any existing nodes in data box
			dataItemTreeView->Nodes->Clear();

			this->dataItemsCopy = gcnew cli::array<FirmwareItem^>(this->firmware->GetDataItemCount());

			// Add nodes to data box
			int current_parent_index = 0;
			for( Int32 i = 0; i < this->dataItemsCopy->Length; i++ )
			{
				this->dataItemsCopy[i] = this->firmware->GetDataItem(i)->Duplicate();

				if( this->dataItemsCopy[i]->GetParentIndex() == i )
				{
					dataItemTreeView->Nodes->Add(this->dataItemsCopy[i]);
					current_parent_index = i;
				}
				else
				{
					dataItemsCopy[current_parent_index]->Nodes->Add( this->dataItemsCopy[i] );
				}
			}

			InitializeGraph();
			graphTime->Start();
			timer1->Start();

		}

	private:

		void PopulateGraphSettings()
		{
			this->checkBox_autoSetRange->Checked = this->autoSetAxes;
			this->textBox_historySize->Text = this->dataHistorySize.ToString();
			this->textBox_title->Text = this->title;
			this->textBox_xlabel->Text = this->xLabel;
			this->textBox_ylabel->Text = this->yLabel;
			this->textBox_yMax->Text = this->yMax.ToString();
			this->textBox_yMin->Text = this->yMin.ToString();						
		}

		void RetrieveGraphSettingsFromDialog()
		{
			this->title = this->textBox_title->Text;
			this->xLabel = this->textBox_xlabel->Text;
			this->yLabel = this->textBox_ylabel->Text;
			try
			{
				this->yMax = double::Parse(this->textBox_yMax->Text);
			}
			catch( Exception^ /*e*/ )
			{
				this->textBox_yMax->Text = this->yMax.ToString();
			}
			try
			{
				this->yMin = double::Parse(this->textBox_yMin->Text);
			}
			catch( Exception^ /*e*/ )
			{
				this->textBox_yMin->Text = this->yMin.ToString();
			}
			this->autoSetAxes = this->checkBox_autoSetRange->Checked;
			try
			{
				this->dataHistorySize = int::Parse(this->textBox_historySize->Text);
			}
			catch( Exception^ /*e*/ )
			{
				this->textBox_historySize->Text = this->dataHistorySize.ToString();
			}
		}

		void ClearGraph()
		{
			// Remove curves from graph pane
			this->graphControl->GraphPane->CurveList->Clear();

			this->dataListCount = 0;
		}


		void InitializeGraph()
		{
			this->graphControl->GraphPane->Title->Text = this->title;
			this->graphControl->GraphPane->XAxis->Title->Text = this->xLabel;
			this->graphControl->GraphPane->YAxis->Title->Text = this->yLabel;			

			// If autoSetRange isn't checked
			if( !this->autoSetAxes )
			{
				this->graphControl->GraphPane->YAxis->Scale->Max = this->yMax;
				this->graphControl->GraphPane->YAxis->Scale->Min = this->yMin;
			}
			else
			{
				// TODO: figure out how to make the graph auto scale and add that here
			}

			// Iterate through the data item list and find items that are selected.  For each one, add a new list to the graph pane
			for( int i = 0; i < dataItemsCopy->Length; i++ )
			{
				// Ignore parent items that are checked
				if( dataItemsCopy[i]->GetParentIndex() != i )
				{
					// Check to see if the current item is checked
					if( dataItemsCopy[i]->Checked )
					{
						// Item is checked.  Add new list view to graph
						dataGraphList[dataListCount] = gcnew RollingPointPairList(this->dataHistorySize);

						this->dataItemIndexes[dataListCount] = i;

						Color next_color = colors[dataListCount % COLOR_COUNT];
						this->graphControl->GraphPane->AddCurve(dataItemsCopy[i]->Text, dataGraphList[dataListCount], next_color, SymbolType::None);
						dataListCount++;
					}
				}
			}

			RefreshGraph();
		}

		void RefreshGraph()
        {
            graphControl->AxisChange();
            graphControl->Invalidate();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~DataGraphDialog()
		{
			if (components)
			{
				delete components;
			}
		}
	// <adeel>
	private: SharedMemWrapper^ IMUSharedMemory;
	private: MutexObjWrapper^ IMUSharedMemoryMutex;
	// <Ahmed>
	private: EventObjWrapper^ IMUSharedMemoryEvent;
	bool writeOneShot;
	// </Ahmed>
	// </adeel>
	private: ZedGraph::ZedGraphControl^  graphControl;
	private: System::Windows::Forms::TabControl^  tabControl;

	private: cli::array<RollingPointPairList^>^ dataGraphList;
	private: UInt32 dataListCount;

	private: System::Windows::Forms::TabPage^  tabPage1;
	private: System::Windows::Forms::TabPage^  tabPage2;
	private: System::Windows::Forms::TabPage^  tabPage3;
	private: System::Windows::Forms::TreeView^  dataItemTreeView;
	private: System::Windows::Forms::TextBox^  textBox_title;
	private: FirmwareDefinition^ firmware;
	private: cli::array<FirmwareItem^>^ dataItemsCopy;
	private: cli::array<UInt32,1>^ dataItemIndexes;

	private: cli::array<Color>^ colors;

	private: Stopwatch^ graphTime;
	
	private: bool dataSendingEnabled;

	private: bool autoSetAxes;
	private: String^ title;
	private: String^ xLabel;
	private: String^ yLabel;
	private: int dataHistorySize;
	private: double yMin;
	private: double yMax;
	private: double time;


	private: System::Windows::Forms::Label^  staticText1;
	private: System::Windows::Forms::TextBox^  textBox_xlabel;

	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::CheckBox^  checkBox_autoSetRange;

	private: System::Windows::Forms::TextBox^  textBox_ylabel;

	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::TextBox^  textBox_yMax;

	private: System::Windows::Forms::TextBox^  textBox_yMin;
	private: System::Windows::Forms::Button^  button_applyChanges;


	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::TextBox^  textBox_historySize;

	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Timer^  timer1;
private: System::Windows::Forms::TabPage^  tabPage4;


private: System::Windows::Forms::SaveFileDialog^  saveFileDialog1;
private: System::Windows::Forms::Button^  stopSendingDataButton;

private: System::Windows::Forms::Button^  startSendingDataButton;


	protected: 
	private: System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(DataGraphDialog::typeid));
			this->graphControl = (gcnew ZedGraph::ZedGraphControl());
			this->tabControl = (gcnew System::Windows::Forms::TabControl());
			this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
			this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
			this->dataItemTreeView = (gcnew System::Windows::Forms::TreeView());
			this->tabPage3 = (gcnew System::Windows::Forms::TabPage());
			this->textBox_historySize = (gcnew System::Windows::Forms::TextBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->button_applyChanges = (gcnew System::Windows::Forms::Button());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->textBox_ylabel = (gcnew System::Windows::Forms::TextBox());
			this->textBox_xlabel = (gcnew System::Windows::Forms::TextBox());
			this->textBox_title = (gcnew System::Windows::Forms::TextBox());
			this->staticText1 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->checkBox_autoSetRange = (gcnew System::Windows::Forms::CheckBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->textBox_yMax = (gcnew System::Windows::Forms::TextBox());
			this->textBox_yMin = (gcnew System::Windows::Forms::TextBox());
			this->tabPage4 = (gcnew System::Windows::Forms::TabPage());
			this->stopSendingDataButton = (gcnew System::Windows::Forms::Button());
			this->startSendingDataButton = (gcnew System::Windows::Forms::Button());
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->saveFileDialog1 = (gcnew System::Windows::Forms::SaveFileDialog());
			this->tabControl->SuspendLayout();
			this->tabPage1->SuspendLayout();
			this->tabPage2->SuspendLayout();
			this->tabPage3->SuspendLayout();
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->tabPage4->SuspendLayout();
			this->SuspendLayout();
			// 
			// graphControl
			// 
			this->graphControl->Location = System::Drawing::Point(-4, 0);
			this->graphControl->Name = L"graphControl";
			this->graphControl->ScrollGrace = 0;
			this->graphControl->ScrollMaxX = 0;
			this->graphControl->ScrollMaxY = 0;
			this->graphControl->ScrollMaxY2 = 0;
			this->graphControl->ScrollMinX = 0;
			this->graphControl->ScrollMinY = 0;
			this->graphControl->ScrollMinY2 = 0;
			this->graphControl->Size = System::Drawing::Size(354, 183);
			this->graphControl->TabIndex = 0;
			// 
			// tabControl
			// 
			this->tabControl->Controls->Add(this->tabPage1);
			this->tabControl->Controls->Add(this->tabPage2);
			this->tabControl->Controls->Add(this->tabPage3);
			this->tabControl->Controls->Add(this->tabPage4);
			this->tabControl->Location = System::Drawing::Point(0, 0);
			this->tabControl->Name = L"tabControl";
			this->tabControl->SelectedIndex = 0;
			this->tabControl->Size = System::Drawing::Size(354, 204);
			this->tabControl->TabIndex = 1;
			this->tabControl->Resize += gcnew System::EventHandler(this, &DataGraphDialog::tabControl_Resize);
			// 
			// tabPage1
			// 
			this->tabPage1->Controls->Add(this->graphControl);
			this->tabPage1->Location = System::Drawing::Point(4, 22);
			this->tabPage1->Name = L"tabPage1";
			this->tabPage1->Padding = System::Windows::Forms::Padding(3);
			this->tabPage1->Size = System::Drawing::Size(346, 178);
			this->tabPage1->TabIndex = 0;
			this->tabPage1->Text = L"Graph";
			this->tabPage1->UseVisualStyleBackColor = true;
			// 
			// tabPage2
			// 
			this->tabPage2->Controls->Add(this->dataItemTreeView);
			this->tabPage2->Location = System::Drawing::Point(4, 22);
			this->tabPage2->Name = L"tabPage2";
			this->tabPage2->Padding = System::Windows::Forms::Padding(3);
			this->tabPage2->Size = System::Drawing::Size(346, 178);
			this->tabPage2->TabIndex = 1;
			this->tabPage2->Text = L"Data";
			this->tabPage2->UseVisualStyleBackColor = true;
			// 
			// dataItemTreeView
			// 
			this->dataItemTreeView->CheckBoxes = true;
			this->dataItemTreeView->Location = System::Drawing::Point(0, 0);
			this->dataItemTreeView->Name = L"dataItemTreeView";
			this->dataItemTreeView->Size = System::Drawing::Size(350, 182);
			this->dataItemTreeView->TabIndex = 0;
			this->dataItemTreeView->AfterCheck += gcnew System::Windows::Forms::TreeViewEventHandler(this, &DataGraphDialog::dataItemTreeView_AfterCheck);
			// 
			// tabPage3
			// 
			this->tabPage3->Controls->Add(this->textBox_historySize);
			this->tabPage3->Controls->Add(this->label5);
			this->tabPage3->Controls->Add(this->button_applyChanges);
			this->tabPage3->Controls->Add(this->groupBox1);
			this->tabPage3->Controls->Add(this->groupBox2);
			this->tabPage3->Location = System::Drawing::Point(4, 22);
			this->tabPage3->Name = L"tabPage3";
			this->tabPage3->Padding = System::Windows::Forms::Padding(3);
			this->tabPage3->Size = System::Drawing::Size(346, 178);
			this->tabPage3->TabIndex = 2;
			this->tabPage3->Text = L"Settings";
			this->tabPage3->UseVisualStyleBackColor = true;
			this->tabPage3->Click += gcnew System::EventHandler(this, &DataGraphDialog::tabPage3_Click);
			// 
			// textBox_historySize
			// 
			this->textBox_historySize->Location = System::Drawing::Point(25, 139);
			this->textBox_historySize->Name = L"textBox_historySize";
			this->textBox_historySize->Size = System::Drawing::Size(100, 20);
			this->textBox_historySize->TabIndex = 6;
			this->textBox_historySize->TextChanged += gcnew System::EventHandler(this, &DataGraphDialog::textBox_historySize_TextChanged);
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(22, 123);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(123, 13);
			this->label5->TabIndex = 14;
			this->label5->Text = L"History Size (data points)";
			// 
			// button_applyChanges
			// 
			this->button_applyChanges->Enabled = false;
			this->button_applyChanges->Location = System::Drawing::Point(235, 136);
			this->button_applyChanges->Name = L"button_applyChanges";
			this->button_applyChanges->Size = System::Drawing::Size(100, 23);
			this->button_applyChanges->TabIndex = 11;
			this->button_applyChanges->Text = L"Apply Changes";
			this->button_applyChanges->UseVisualStyleBackColor = true;
			this->button_applyChanges->Click += gcnew System::EventHandler(this, &DataGraphDialog::button_applyChanges_Click);
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->textBox_ylabel);
			this->groupBox1->Controls->Add(this->textBox_xlabel);
			this->groupBox1->Controls->Add(this->textBox_title);
			this->groupBox1->Controls->Add(this->staticText1);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Controls->Add(this->label2);
			this->groupBox1->Location = System::Drawing::Point(8, 6);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(201, 107);
			this->groupBox1->TabIndex = 12;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Graph Text";
			// 
			// textBox_ylabel
			// 
			this->textBox_ylabel->Location = System::Drawing::Point(63, 71);
			this->textBox_ylabel->Name = L"textBox_ylabel";
			this->textBox_ylabel->Size = System::Drawing::Size(100, 20);
			this->textBox_ylabel->TabIndex = 5;
			this->textBox_ylabel->TextChanged += gcnew System::EventHandler(this, &DataGraphDialog::textBox_ylabel_TextChanged);
			// 
			// textBox_xlabel
			// 
			this->textBox_xlabel->Location = System::Drawing::Point(63, 45);
			this->textBox_xlabel->Name = L"textBox_xlabel";
			this->textBox_xlabel->Size = System::Drawing::Size(100, 20);
			this->textBox_xlabel->TabIndex = 3;
			this->textBox_xlabel->TextChanged += gcnew System::EventHandler(this, &DataGraphDialog::textBox_xlabel_TextChanged);
			// 
			// textBox_title
			// 
			this->textBox_title->Location = System::Drawing::Point(63, 18);
			this->textBox_title->Name = L"textBox_title";
			this->textBox_title->Size = System::Drawing::Size(100, 20);
			this->textBox_title->TabIndex = 1;
			this->textBox_title->TextChanged += gcnew System::EventHandler(this, &DataGraphDialog::textBox_title_TextChanged);
			// 
			// staticText1
			// 
			this->staticText1->AutoSize = true;
			this->staticText1->Location = System::Drawing::Point(30, 22);
			this->staticText1->Name = L"staticText1";
			this->staticText1->Size = System::Drawing::Size(27, 13);
			this->staticText1->TabIndex = 0;
			this->staticText1->Text = L"Title";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(14, 48);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(43, 13);
			this->label1->TabIndex = 2;
			this->label1->Text = L"X Label";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(14, 75);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(43, 13);
			this->label2->TabIndex = 4;
			this->label2->Text = L"Y Label";
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->checkBox_autoSetRange);
			this->groupBox2->Controls->Add(this->label3);
			this->groupBox2->Controls->Add(this->label4);
			this->groupBox2->Controls->Add(this->textBox_yMax);
			this->groupBox2->Controls->Add(this->textBox_yMin);
			this->groupBox2->Location = System::Drawing::Point(216, 7);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(130, 106);
			this->groupBox2->TabIndex = 13;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Range";
			// 
			// checkBox_autoSetRange
			// 
			this->checkBox_autoSetRange->AutoSize = true;
			this->checkBox_autoSetRange->Checked = true;
			this->checkBox_autoSetRange->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBox_autoSetRange->Location = System::Drawing::Point(12, 17);
			this->checkBox_autoSetRange->Name = L"checkBox_autoSetRange";
			this->checkBox_autoSetRange->Size = System::Drawing::Size(95, 17);
			this->checkBox_autoSetRange->TabIndex = 6;
			this->checkBox_autoSetRange->Text = L"Auto-set range";
			this->checkBox_autoSetRange->UseVisualStyleBackColor = true;
			this->checkBox_autoSetRange->CheckedChanged += gcnew System::EventHandler(this, &DataGraphDialog::autoScaleCheckBox_CheckedChanged);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(16, 47);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(34, 13);
			this->label3->TabIndex = 7;
			this->label3->Text = L"Y Min";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(13, 74);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(37, 13);
			this->label4->TabIndex = 8;
			this->label4->Text = L"Y Max";
			// 
			// textBox_yMax
			// 
			this->textBox_yMax->Enabled = false;
			this->textBox_yMax->Location = System::Drawing::Point(56, 71);
			this->textBox_yMax->Name = L"textBox_yMax";
			this->textBox_yMax->Size = System::Drawing::Size(63, 20);
			this->textBox_yMax->TabIndex = 10;
			this->textBox_yMax->TextChanged += gcnew System::EventHandler(this, &DataGraphDialog::textBox_yMax_TextChanged);
			// 
			// textBox_yMin
			// 
			this->textBox_yMin->Enabled = false;
			this->textBox_yMin->Location = System::Drawing::Point(56, 44);
			this->textBox_yMin->Name = L"textBox_yMin";
			this->textBox_yMin->Size = System::Drawing::Size(63, 20);
			this->textBox_yMin->TabIndex = 9;
			this->textBox_yMin->TextChanged += gcnew System::EventHandler(this, &DataGraphDialog::textBox_yMin_TextChanged);
			// 
			// tabPage4
			// 
			this->tabPage4->Controls->Add(this->stopSendingDataButton);
			this->tabPage4->Controls->Add(this->startSendingDataButton);
			this->tabPage4->Location = System::Drawing::Point(4, 22);
			this->tabPage4->Name = L"tabPage4";
			this->tabPage4->Padding = System::Windows::Forms::Padding(3);
			this->tabPage4->Size = System::Drawing::Size(346, 178);
			this->tabPage4->TabIndex = 3;
			this->tabPage4->Text = L"Send Data";
			this->tabPage4->UseVisualStyleBackColor = true;
			// 
			// stopSendingDataButton
			// 
			this->stopSendingDataButton->Enabled = false;
			this->stopSendingDataButton->Location = System::Drawing::Point(142, 6);
			this->stopSendingDataButton->Name = L"stopSendingDataButton";
			this->stopSendingDataButton->Size = System::Drawing::Size(88, 34);
			this->stopSendingDataButton->TabIndex = 3;
			this->stopSendingDataButton->Text = L"Stop Sending Data";
			this->stopSendingDataButton->UseVisualStyleBackColor = true;
			this->stopSendingDataButton->Click += gcnew System::EventHandler(this, &DataGraphDialog::stopSendingDataButton_Click);
			// 
			// startSendingDataButton
			// 
			this->startSendingDataButton->Location = System::Drawing::Point(8, 6);
			this->startSendingDataButton->Name = L"startSendingDataButton";
			this->startSendingDataButton->Size = System::Drawing::Size(84, 34);
			this->startSendingDataButton->TabIndex = 2;
			this->startSendingDataButton->Text = L"Start Sending Data";
			this->startSendingDataButton->UseVisualStyleBackColor = true;
			this->startSendingDataButton->Click += gcnew System::EventHandler(this, &DataGraphDialog::startSendingDataButton_Click);
			// 
			// timer1
			// 
			this->timer1->Tick += gcnew System::EventHandler(this, &DataGraphDialog::timer1_Tick);
			// 
			// saveFileDialog1
			// 
			this->saveFileDialog1->Filter = L"CSV Files (*.csv)|*.csv|Log Files (*.log)|*.log|Text Files (*.txt)|*.txt|All File" 
				L"s (*.*)|*.*";
			// 
			// DataGraphDialog
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(354, 207);
			this->Controls->Add(this->tabControl);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->MinimumSize = System::Drawing::Size(370, 243);
			this->Name = L"DataGraphDialog";
			this->Text = L"Data Graph";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &DataGraphDialog::DataGraphDialog_FormClosing_1);
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &DataGraphDialog::DataGraphDialog_FormClosed);
			this->Load += gcnew System::EventHandler(this, &DataGraphDialog::DataGraphDialog_Load);
			this->Resize += gcnew System::EventHandler(this, &DataGraphDialog::DataGraphDialog_Resize);
			this->tabControl->ResumeLayout(false);
			this->tabPage1->ResumeLayout(false);
			this->tabPage2->ResumeLayout(false);
			this->tabPage3->ResumeLayout(false);
			this->tabPage3->PerformLayout();
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			this->tabPage4->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void DataGraphDialog_Resize(System::Object^  sender, System::EventArgs^  e) {
				 this->tabControl->Size = this->ClientSize;
			 }
			 // When form is closed, unload event handlers, change back to the default size, etc.
	private: System::Void DataGraphDialog_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
			 }
	private: System::Void DataGraphDialog_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e) {
				 
			 }
	private: System::Void DataGraphDialog_Load(System::Object^  sender, System::EventArgs^  e) {
			 }
private: System::Void tabControl_Resize(System::Object^  sender, System::EventArgs^  e) {		 
			 this->graphControl->Size = this->tabControl->SelectedTab->ClientSize;
			 this->dataItemTreeView->Size = this->tabControl->SelectedTab->ClientSize;
		 }
private: System::Void tabPage3_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void autoScaleCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
		 {
			 if( this->checkBox_autoSetRange->Checked )
			 {
				this->textBox_yMax->Enabled = false;
				this->textBox_yMin->Enabled = false;
			 }			
			 else
			 {
				this->textBox_yMax->Enabled = true;
				this->textBox_yMin->Enabled = true;
			 }

			 this->button_applyChanges->Enabled = true;

		 }
private: System::Void dataItemTreeView_AfterCheck(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e) 
		 {
			 // Clear and update the graph
			 this->ClearGraph();
			 this->InitializeGraph();
		 }
private: System::Void timer1_Tick(System::Object^  sender, System::EventArgs^  e) 
		 {
			 this->graphTime->Stop();

			 IMU_Raw_Data imu_data = {};
			 			 
			 time = this->graphTime->Elapsed;
			 imu_data.time_s = time;

			 // Update graph contents based on most recently received data
			 for( UInt32 i = 0; i < this->dataListCount; i++ )
			 {
				 FirmwareItem^ current_item = this->firmware->GetDataItem(this->dataItemIndexes[i]);
				 String^ data_type = current_item->GetDataType();
				 double data;
				 if( data_type == L"float" )
				 {
					 data = (double)current_item->GetFloatData()*(double)current_item->GetScaleFactor();
				 }
				 else if( data_type == L"int16" || data_type == L"int32" )
				 {
					 data = (double)current_item->GetIntData()*(double)current_item->GetScaleFactor();
				 }
				 else if( data_type == L"uint16" || data_type == L"uint32" )
				 {
					 data = (double)current_item->GetUIntData()*(double)current_item->GetScaleFactor();
				 }
				 else if( data_type == L"binary" || data_type == L"en/dis" )
				 {
					 bool value = current_item->GetBinaryData();
					 if( value )
						 data = 1.0;
					 else
						 data = 0.0;
				 }
				 else
				 {
					 data = -1.0;
				 }

				 // <adeel>
				 /*
				 if( i == 0 )
				 {
					 if( ( data >= ( double )( PROCESSED_X_GYRO_NOISE_RANGE_LOW_THRESHOLD ) ) &&
						 ( data <= ( double )( PROCESSED_X_GYRO_NOISE_RANGE_HIGH_THRESHOLD ) ) )
					 {
						 data = 0;
					 }
					 else if( data < ( double )( PROCESSED_X_GYRO_NOISE_RANGE_LOW_THRESHOLD ) )
					 {
						 data -= PROCESSED_X_GYRO_NOISE_RANGE_LOW_THRESHOLD;
					 }
					 else
					 {
						 data -= PROCESSED_X_GYRO_NOISE_RANGE_HIGH_THRESHOLD;
					 }
				 }
				 */

				 /*
				 if( i == 0 )
				 {
					 if( ( data >= (double)(PROCESSED_X_GYRO_NOISE_RANGE_LOW_THRESHOLD) ) &&
						 ( data <= (double)(PROCESSED_X_GYRO_NOISE_RANGE_HIGH_THRESHOLD) ) )
					 {
						 data = 0;
					 }
				 }
				 else if( i == 1 )
				 {
					 if( ( data >= (double)(PROCESSED_Y_GYRO_NOISE_RANGE_LOW_THRESHOLD) ) &&
						 ( data <= (double)(PROCESSED_Y_GYRO_NOISE_RANGE_HIGH_THRESHOLD) ) )
					 {
						 data = 0;
					 }
				 }
				 */
				 // </adeel>

				 this->dataGraphList[i]->Add(time, data);

				 // <Ahmed>
				 writeOneShot = false;
				 switch(i){
				 case 0:
					 imu_data.x_gyro = data;
					 break;
				 case 1:
					 imu_data.y_gyro = data;
					 break;
				 case 2:
					 imu_data.z_gyro = data;
					 break;
				 case 3:
					 imu_data.x_accel = data;
					 break;
				 case 4:
					 imu_data.y_accel = data;
					 break;
				 case 5:
					 imu_data.z_accel = data;
					 writeOneShot = true;
					 break;
				 default:
					 break;
				 }
				 // </Ahmed>
				 
				 // <adeel>
				 if( ( dataSendingEnabled ) && writeOneShot)
					 //( i < 2 ) )
				 {
					 
					 if( IMUSharedMemoryMutex->lockMutexWrapper(5) )
					 {
						 IMUSharedMemory->writeBytes((const void *)(&imu_data),
												 sizeof(IMU_Raw_Data),
												 0);
					 }
					 IMUSharedMemoryEvent->setEventWrapper();
					 IMUSharedMemoryMutex->unlockMutexWrapper();

					/* 
					float floatData = (float)(data);
					 IMUSharedMemoryMutex->lockMutexWrapper(INFINITE_WAIT);
					 IMUSharedMemory->writeBytes((const void *)(&floatData),
												 sizeof(floatData),
												 i*sizeof(floatData));
					 IMUSharedMemoryMutex->unlockMutexWrapper();
					 */

				 }
				 // </adeel>

			 }

			 this->RefreshGraph();
		 }
private: System::Void button_applyChanges_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 this->RetrieveGraphSettingsFromDialog();
			 this->ClearGraph();
			 this->InitializeGraph();

			 this->button_applyChanges->Enabled = false;
		 }
private: System::Void textBox_yMin_TextChanged(System::Object^  sender, System::EventArgs^  e) 
		 {
			 this->button_applyChanges->Enabled = true;
		 }
private: System::Void textBox_yMax_TextChanged(System::Object^  sender, System::EventArgs^  e) 
		 {
			 this->button_applyChanges->Enabled = true;
		 }
private: System::Void textBox_historySize_TextChanged(System::Object^  sender, System::EventArgs^  e) 
		 {
			 this->button_applyChanges->Enabled = true;
		 }
private: System::Void textBox_title_TextChanged(System::Object^  sender, System::EventArgs^  e) 
		 {
			 this->button_applyChanges->Enabled = true;
		 }
private: System::Void textBox_xlabel_TextChanged(System::Object^  sender, System::EventArgs^  e) 
		 {
			 this->button_applyChanges->Enabled = true;
		 }
private: System::Void textBox_ylabel_TextChanged(System::Object^  sender, System::EventArgs^  e) 
		 {
			 this->button_applyChanges->Enabled = true;
		 }
private: System::Void startSendingDataButton_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 this->dataSendingEnabled = true;
			 this->startSendingDataButton->Enabled = false;
			 this->stopSendingDataButton->Enabled = true;
		}
private: System::Void stopSendingDataButton_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 this->dataSendingEnabled = false;
			 this->startSendingDataButton->Enabled = true;
			 this->stopSendingDataButton->Enabled = false;
		 }
private: System::Void DataGraphDialog_FormClosing_1(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
			 dataSendingEnabled = false;
		 }
};
}
