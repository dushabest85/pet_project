
//Если не xотим хранить в векторе копии, то можем хранить ссылки на исходный объект
/*
std::vector<std::reference_wrapper<const ColumnMetaData<WorkColumnTypes::Type>>> combineColumns;

for(auto it = template_->beginColumn(); it != template_->endColumn(); ++it)
    boost::range::copy(it->columns, std::back_inserter(combineColumns));
*/
