#include "binaryninja-api/ui/uicontext.h"
#include "binaryninjaapi.h"
#include "ui/action.h"
#include "ui/filecontext.h"
#include "ui/tabwidget.h"
#include "ui/uicontext.h"
#include "ui/linearview.h"
#include "ui/uitypes.h"
#include "ui/viewframe.h"
#include <algorithm>
#include <iterator>
#include <qabstractitemmodel.h>
#include <qevent.h>
#include <qmainwindow.h>
#include <qaction.h>
#include <qnamespace.h>
#include <qtmetamacros.h>
#include <qtreeview.h>
#include <qvariant.h>
#include <strings.h>
#include <tuple>
#include "ui/disassemblyview.h"


#ifndef SNAPVIEW_H
#define  SNAPVIEW_H

typedef std::pair<std::string, std::string> SnapField;
typedef std::map<std::string, std::variant<int64_t, std::string>> SnapEntry;


class SnapNotif : public UIContextNotification {
public:
    static void init();
};

class SnapViewer : public QDialog {
public:
    SnapViewer(BinaryViewRef bv, UIActionContext ctx);
    static void init();
    static void display(const UIActionContext& action);
private:
    BinaryViewRef m_bv;
    UIActionContext m_ctx;
};

class SnapTree : public QTreeView {
public:
    SnapTree(BinaryViewRef bv, UIActionContext ctx);
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
private:
    BinaryViewRef m_bv;
    UIActionContext m_ctx;
};

class SnapModel : public QAbstractItemModel{
public:
    SnapModel(BinaryViewRef bv);
    QModelIndex parent(const QModelIndex &child) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    // void sort(int col, Qt::SortOrder order = Qt::AscendingOrder) const override;
private:
    BinaryViewRef m_bv;
    std::vector<SnapEntry> m_entries;
    std::vector<SnapField> m_fields;

};

#endif // SNAPVIEW_H
