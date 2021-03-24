// 2020.04.26 - Victor Dods

#pragma once

#include <QTreeView>
#include "sept/Data.hpp"

class SEPTModel;

class SEPTTreeView : public QTreeView {
    Q_OBJECT

public:

    template <typename... Args_>
    SEPTTreeView (Args_&&... args)
    :   QTreeView(std::forward<Args_>(args)...)
    ,   m_source_view(nullptr)
    {
        initialize();
    }

    // Covariant return type
    SEPTModel *model () const;
    SEPTTreeView *source_view () const { return m_source_view; }

    void resize_columns_to_contents ();
    void set_source_view (SEPTTreeView *source_view);
    sept::Data extract_elements () const;
    void integrate_data (sept::Data const &projection_with_complement);

private:

    void initialize ();

    SEPTTreeView *m_source_view;
};
