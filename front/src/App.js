import styles from './index.module.css'
import React, {useEffect, useState} from "react";
import TextareaAutosize from 'react-textarea-autosize';

function App() {
    const [queryValue, setQueryValue] = useState("");
    const [lastQuery, setLastQuery] = useState("");
    const [queryResponse, setQueryResponse] = useState("")
    const [goodResponse, setGoodResponse] = useState(false);

    useEffect(() => {
        console.log(goodResponse)
    }, [goodResponse]);

    function submitQuery() {
        fetch("http://localhost:5431/query", {
            body: queryValue,
            method: "POST",
            mode: "cors",
            headers: {
                'Content-Type': 'text/plain'
            }
        })
            .then(response => {
                if (!response.ok) {
                    return response.text().then(text => {
                        throw new Error(text || 'Some default error message');
                    });
                }
                return response.text();
            })
            .then(data => {
                setQueryResponse(data);
                setGoodResponse(true);
            })
            .catch(reason => {
                console.error('Error:', reason.message); // Log or handle the error message
                setQueryResponse(reason.message);
                setGoodResponse(false);
            });
    }

    const handleKeyDown = (event) => {
        if (event.key === 'Enter' && !event.shiftKey) {
            event.preventDefault();
            submitQuery();
            setLastQuery(queryValue);
            setQueryValue("");
        }
    };

    return (
        <div className="App">
            <div className={styles.header}>
                <div>CrossdressSQL</div>
            </div>

            <span>Request:</span>
            <div className={styles.input_div}>
                <TextareaAutosize
                    value={queryValue}
                    onChange={e => setQueryValue(e.target.value)}
                    onKeyDown={handleKeyDown}
                />
            </div>

            <span>Response:</span>
            <div className={styles.bottom_div} style={{
                color: goodResponse ? "black" : "red",
                whiteSpace: "pre-wrap",
                fontFamily: "Courier New"
            }}>
                {lastQuery.toLowerCase()}
                <br/>
                <br/>
                {queryResponse}
            </div>
        </div>
    );
}

export default App;
