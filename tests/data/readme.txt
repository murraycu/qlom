The sqlite folder contains several Glom documents which all use the sqlite db
backend. The document names loosely represent the intended test case, as each
document is supposed to trigger a specific error handling code path in Qlom.

The following is a short list of documents and the expected response of Qlom.
Please update this list when the error handling changes!

sqlite/empty.glom:
    No default table found (actually, no table at all, but the default table
    check overshadows this one).

sqlite/invalid-db.glom:
    No default table found (actually, no table at all, but the default table
    check overshadows this one).

sqlite/no-list-model.glom:
    No list model found (document specifies a table, but no layout groups).
