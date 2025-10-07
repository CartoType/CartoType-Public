using CartoType;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics.Eventing.Reader;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows.Forms;

namespace CartoTypeDemo
{
    public partial class FindDialog : Form
    {
        public FindDialog()
        {
            InitializeComponent();
            m_find_param.MaxObjectCount = 64;

            // Exclude boundaries, minor admin areas, railway platforms and bus stops.
            m_find_param.Condition = "@feature_type notin { 'bou', 'aae', 'aaf', 'aag', 'aah', 'aai', 'aaj', 'plf', 'bsp' }";

            m_find_param.StringMatchMethod = CartoType.StringMatchMethod.PrefixFlag |
                                             CartoType.StringMatchMethod.FoldAccentsFlag |
                                             CartoType.StringMatchMethod.FoldCaseFlag |
                                             CartoType.StringMatchMethod.IgnoreWhitespaceFlag |
                                             CartoType.StringMatchMethod.IgnoreSymbolsFlag;
            m_find_param.Merge = true;
        }

        public void Set(CartoType.Framework aFramework)
        {
            m_framework = aFramework;

            // Find items in or near the view by preference.
            var view = new CartoType.Rect();
            m_framework.GetView(view, CartoType.CoordType.Map);
            m_find_param.Location = new CartoType.Geometry(view, CartoType.CoordType.Map);
            PopulateList(FindText.Text);
        }

        public MapObjectList FoundObjectList()
        {
            if (FindList.SelectedIndex >= 0 && FindList.SelectedIndex < m_map_object_group_list.Count)
                return m_map_object_group_list[FindList.SelectedIndex].MapObjectList;

            var list = new MapObjectList();
            foreach (var p in m_map_object_group_list)
                foreach (var o in p.MapObjectList)
                    list.Add(o);

            return list;
        }

        private void FindDialog_Shown(object sender, EventArgs e)
        {
            FindText.Focus();
        }

        private void FindText_TextChanged(object sender, EventArgs e)
        {
            PopulateList(FindText.Text);
            FindList.SelectedIndex = -1;
        }

        private void FindText_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                DialogResult = DialogResult.OK;
                Close();
            }
            if (e.KeyCode == Keys.Down)
            {
                FindList.Focus();
                FindList.SelectedIndex = FindList.Items.Count > 0 ? 0 : -1;
            }
        }

        private void Symbols_CheckedChanged(object sender, EventArgs e)
        {
            PopulateList(FindText.Text);
        }

        private void FuzzyMatch_CheckedChanged(object sender, EventArgs e)
        {
            PopulateList(FindText.Text);
        }

        private void FindList_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                DialogResult = DialogResult.OK;
                Close();
            }
        }

        private void FindList_DoubleClick(object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK;
            Close();
        }


        private void FindCallback(MapObjectGroupList aMapObjectGroupList)
        {
            // Use Invoke because this function is called from a thread other than the one in which this dialog was created.
            Invoke(new MethodInvoker(delegate
                {
                    m_map_object_group_list = aMapObjectGroupList;
                    SetListStrings();
                }));
        }

        private void PopulateList(string aText)
        {
            if (aText.Length == 0)
            {
                m_map_object_group_list.Clear();
                SetListStrings();
                return;
            }

            // Find up to 64 items starting with the current text.
            var param = m_find_param;
            param.Text = aText;
            if (FuzzyMatch.Checked)
                param.StringMatchMethod = CartoType.StringMatchMethod.Fuzzy;
            if (Symbols.Checked)
                param.StringMatchMethod -= CartoType.StringMatchMethod.IgnoreSymbolsFlag;
            m_framework.FindAsync(FindCallback, param, true);
        }

        void SetListStrings()
        {
            FindList.Items.Clear();
            foreach (var cur_group in m_map_object_group_list)
            {
                FindList.Items.Add(cur_group.Name);
            }
        }

        private CartoType.Framework m_framework;
        private CartoType.FindParam m_find_param = new();
        private CartoType.MapObjectGroupList m_map_object_group_list = [];
    }

}
